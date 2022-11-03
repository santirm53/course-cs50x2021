-- In 13.sql, write a SQL query to list the names of all people who starred in a movie in which Kevin Bacon also starred.
-- Your query should output a table with a single column for the name of each person.
-- There may be multiple people named Kevin Bacon in the database. Be sure to only select the Kevin Bacon born in 1958.
-- Kevin Bacon himself should not be included in the resulting list.

--Get name of that people who have an id ...:
SELECT people.name FROM people WHERE people.id IN
    -- Which appears in the people_id of the stars that share a same movie id...:
    (SELECT person_id FROM stars WHERE movie_ID IN
        -- With those in wich Kevin Bacon (1958) stars.
        (SELECT movies.id FROM movies
        JOIN stars ON stars.movie_id = movies.id
        JOIN people ON people.id = stars.person_id
        WHERE ((people.name = "Kevin Bacon") AND (people.birth = 1958))
        )
    )
-- and that are not named Kevin Bacon:
AND people.name != "Kevin Bacon";
