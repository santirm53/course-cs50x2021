-- Keep a log of any SQL queries you execute as you solve the mystery.
-- General view
.tables

-- General view of reports
.schema crime_scene_reports
SELECT * FROM crime_scene_reports

-- Filter reports mentioning courthouse (3 of them according to report)
SELECT id, transcript, year, month, day FROM interviews WHERE transcript LIKE "%courthouse%";
-- All 3 of them regarding our case are from july so this is just cleaner.
SELECT id, transcript FROM interviews WHERE (transcript LIKE "%courthouse%") AND month = 7;

-- Now 3 paths open up:
    --atm_transaction, withdrawal at Fifer Street 7/28/2020.
    SELECT * FROM atm_transactions WHERE (year = 2020 AND month = 7 AND day = 28 AND transaction_type = "withdraw" AND atm_location = "Fifer Street");

    --footage of car leaving the courthouse parking lot 10' after the theft. So 10:15 to 10:25 according to the crime_scene_reports
    SELECT * FROM courthouse_security_logs WHERE (year = 2020 AND month = 7 AND day = 28 AND activity = "exit" AND hour = 10 AND (minute >= 15 AND minute <=25));

    --call as he was leaving, less than a minute, saying he would take the earliest flight on the 29th.
    SELECT * FROM flights WHERE ((year = 2020) AND (month = 7) AND (day = 29)) ORDER BY hour ASC; -- This is the earliest flight the next day. FLIGHT ID = 36
        --knowing the exact time of the flight we can get the origin id (8) and the destination id (4)
        SELECT * FROM airports WHERE (id = 8) OR (id = 4) --so he escaped From CSF (fiftyville) to LHR (London)
            -- We can also get the possible passport_numbers
            SELECT * FROM passengers WHERE flight_id = 36;
        --now for the call
        SELECT caller, receiver FROM phone_calls WHERE year = 2020 AND month = 7 AND day = 28 AND duration < 60; -- this are the possible perpetrator and accomplice.

-- from the table people, isolate those whose phone number is in the list and whose passport number is in the flight:
SELECT * FROM people
WHERE
((
    passport_number IN
    (SELECT passport_number FROM passengers WHERE flight_id = 36)
)
AND (phone_number IN (SELECT caller FROM phone_calls WHERE year = 2020 AND month = 7 AND day = 28 AND duration < 60)
));

-- Same logic, I just noticed looking at the table I can find the intersection of phone numbers from the calls, passport from flights and license from footage...
SELECT * FROM people
WHERE (
    passport_number IN (SELECT passport_number FROM passengers WHERE flight_id = 36)
    AND
    (phone_number IN (SELECT caller FROM phone_calls WHERE year = 2020 AND month = 7 AND day = 28 AND duration < 60))
    AND
    (license_plate IN (SELECT license_plate FROM courthouse_security_logs WHERE (year = 2020 AND month = 7 AND day = 28 AND activity = "exit")))
);
-- Roger, Madison, Evelyn or Ernest.
-- Maybe using the bank account?

SELECT name FROM people
JOIN bank_accounts ON bank_accounts.person_id = people.id
WHERE bank_accounts.account_number IN
(SELECT account_number FROM atm_transactions WHERE ((year = 2020) AND (month = 7) AND (day = 28) AND (transaction_type = "withdraw") AND (atm_location = "Fifer Street")));

-- And the possible names are: Ernest, Russel, Roy, Bobby, Elizabeth, Danielle, Madison, Victoria.

--This two lists only intersect at Ernest.
SELECT name FROM people
JOIN bank_accounts ON bank_accounts.person_id = people.id
WHERE (
passport_number IN (SELECT passport_number FROM passengers WHERE flight_id = 36)
AND
(phone_number IN (SELECT caller FROM phone_calls WHERE year = 2020 AND month = 7 AND day = 28 AND duration < 60))
AND
(license_plate IN (SELECT license_plate FROM courthouse_security_logs WHERE (year = 2020 AND month = 7 AND day = 28 AND activity = "exit" AND hour = 10 AND (minute >= 15 AND minute <=25))))
AND
(bank_accounts.account_number IN (SELECT account_number FROM atm_transactions WHERE (year = 2020 AND month = 7 AND day = 28 AND transaction_type = "withdraw" AND atm_location = "Fifer Street")))
);

-- This means Ernest:
    --Are fliying on flight 36 to Heathrow London on the 29th.
    --Called someone the day of the theft for less than a minute.
    --Exited the courthouse parking lot the day of the theft.
    --Withdrew money that day from an ATM in Fifer Street.

-- Our suspects ID:
SELECT * FROM people WHERE name = "Ernest";

-- For the accomplice we need the receiver of that same call, now that we have Earnest personal number: (367) 555-5533
SELECT receiver FROM phone_calls WHERE (caller = "(367) 555-5533" AND year = 2020 AND month = 7 AND day = 28 AND duration < 60);
-- Now we check who has that number from the people table:
SELECT name FROM people WHERE phone_number = (SELECT receiver FROM phone_calls WHERE (caller = "(367) 555-5533" AND year = 2020 AND month = 7 AND day = 28 AND duration < 60));
-- It's Berthold.


