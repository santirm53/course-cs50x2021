import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # I want to pass a completed list of dicts to index.html
    # From portfolios get symbol, company and shares.
    # From users get the remaining cash.
    indexinfo = db.execute("SELECT symbol, company, shares FROM portfolios WHERE user_id=:user_id", user_id=session["user_id"])
    remcash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])

    totalsum = 0

    # Loop through the list I already have, with all the companies the user has stocks of.
    for row in range(len(indexinfo)):
        # Check the current price and add it to dict.
        checkapi = lookup(indexinfo[row]["symbol"])
        price = checkapi["price"]
        indexinfo[row]["price"] = price

        # Multiply to get total and add it to dict.
        total = price * indexinfo[row]["shares"]
        indexinfo[row]["total"] = total

        # Update totalsum variable
        totalsum += total

    # Get remaining cash
    cash = remcash[0]["cash"]
    totalsum += cash

    return render_template("index.html", indexinfo=indexinfo, cash=cash, totalsum=totalsum)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # Render the buy form
    if request.method == "GET":
        return render_template("buy.html")

    # Submit and proceed to the exchange
    else:

        # Form validation: no symbol
        if not request.form.get("symbol"):
            return apology("must provide stock symbol", 400)
        # Form validation: symbol doesn't exist
        symbol = request.form.get("symbol")
        if lookup(symbol) == None:
            return apology("stock not found", 400)

        # Form validation: no shares
        if not request.form.get("shares"):
            return apology("must provide number of shares", 400)
        # Form validation: shares must be a positive int
        shares = request.form.get("shares")

        if shares.isdigit() != True:
            return apology("must provide valid number of shares", 400)

        shares = int(shares)

        # Get the total cash needed and the cash the user has:
        price = lookup(symbol)["price"]
        company = lookup(symbol)["name"]
        symbol = lookup(symbol)["symbol"]
        total = price * shares
        usercash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])
        usercash = usercash[0]["cash"]

        # Not enough cash:
        if total > usercash:
            return apology("not enough cash", 400)

        usercash_after = usercash - total

        # Now that the buy submit is validated, we must execute the transaction and update the corresponding tables. Insert a new transaction:
        db.execute("INSERT INTO transactions (transaction_type, user_id, symbol, stock_price, shares, total_transaction, cash_previous, cash_after) VALUES ('BUY', :user_id, :symbol, :stock_price, :shares, :total_transaction, :cash_previous, :cash_after)",
                   user_id=session["user_id"], symbol=symbol, stock_price=price, shares=shares, total_transaction=total, cash_previous=usercash, cash_after=usercash_after)

        # Look for the company and user in the portfolios table:
        check_portfolio = db.execute(
            "SELECT rowid, symbol, shares FROM portfolios WHERE symbol == :symbol AND user_id == :user_id", symbol=symbol, user_id=session["user_id"])

        if len(check_portfolio) != 1:

            # If the user doesn't have shares from that company, insert it.
            db.execute("INSERT INTO portfolios (user_id, symbol, company, shares) VALUES (:user_id, :symbol, :company, :shares)",
                       user_id=session["user_id"], symbol=symbol, company=company, shares=shares)

        else:

            # If the user already has stocks for that company, update the value.
            updatedshares = check_portfolio[0]["shares"] + shares
            db.execute("UPDATE portfolios SET shares = :updatedshares WHERE rowid = :currentrow",
                       updatedshares=updatedshares, currentrow=check_portfolio[0]["rowid"])

        # Update user remaining cash:
        db.execute("UPDATE users SET cash = :cash_after WHERE id = :user_id", cash_after=usercash_after, user_id=session["user_id"])

        return redirect("/")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    output = db.execute(
        "SELECT transaction_type, symbol, stock_price, shares, cash_previous, cash_after, total_transaction, time FROM transactions WHERE user_id = :user_id ORDER BY time DESC", user_id=session["user_id"])
    return render_template("history.html", output=output)


@app.route("/add", methods=["GET", "POST"])
@login_required
def add_cash():
    """Add cash to the account"""

    # Render a simple adding form.
    if request.method == "GET":
        return render_template("add.html")

    # When reached through POST:
    else:

        # Get current cash:
        cash_current = db.execute("SELECT cash FROM users WHERE id=:user_id", user_id=session["user_id"])
        cash_current = cash_current[0]["cash"]

        # Form validation, empty field:
        if not request.form.get("cash"):
            return apology("must provide amount", 400)

        # Form validation: not a positive number (already  filtered numbers with the HTML form)
        cash_add = int(request.form.get("cash"))
        if cash_add <= 0:
            return apology("must provide valid amount", 400)

        # Update db:
        cash_new = cash_current + cash_add
        db.execute("UPDATE users SET cash = :cash_new WHERE id = :user_id", cash_new=cash_new, user_id=session["user_id"])

        return redirect("/")


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # Render the form when method == GET
    if request.method == "GET":
        return render_template("quote.html")

    # When submitting with POST method
    else:

        # Validation: not stock or not in API results:
        if not request.form.get("symbol"):
            return apology("must provide stock symbol", 400)
        symbol = request.form.get("symbol")
        if lookup(symbol) == None:
            return apology("stock not found", 400)

        # Format to usd the value pair for price key in resulting dict.
        result = usd(lookup(symbol)["price"])

        # Return new template and fill with Jinja placeholders:
        return render_template("quoted.html", symbol=symbol, price=result)


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached register via POST
    if request.method == "POST":

        # Username validation - not username:
        if not request.form.get("username"):
            return apology("must provide username", 400)
        # Username validation - already used:
        # Get a list of dictionaries
        username_list = db.execute("SELECT * FROM users WHERE username = :username", username=(request.form.get("username")))
        # Loop through the list:
        for dictionary in username_list:
            # Use .values() to check the dictionary for same username:
            if request.form.get("username") in dictionary.values():
                return apology("username already exists", 400)

        # Password validation:
        if not request.form.get("password"):
            return apology("must provide password", 400)
        if not request.form.get("confirmation"):
            return apology("must provide confirmation", 400)
        if request.form.get("password") != request.form.get("confirmation"):
            return apology("password and confirmation must match", 400)

        username = request.form.get("username")
        password = request.form.get("password")
        hashed_password = generate_password_hash(password)

        # Insert into database
        sql = "INSERT INTO users (username, hash) VALUES (?, ?)"
        db.execute(sql, username, hashed_password)

        return redirect("/login")

    # User reached register via GET
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # Render the sell form, pass it current stocks owned
    output = db.execute("SELECT symbol FROM portfolios WHERE user_id=:user_id", user_id=session["user_id"])

    stocklist = []
    for row in range(0, len(output)):
        stocklist.append(output[row]["symbol"])

    if request.method == "GET":
        return render_template("sell.html", stocklist=stocklist)

    # Submit and proceed to the exchange
    else:

        # Form validation: no symbol
        if not request.form.get("symbol"):
            return apology("must provide stock symbol", 400)
        # Form validation: symbol doesn't exist
        symbol = request.form.get("symbol")
        if lookup(symbol) == None:
            return apology("stock not found", 400)

        # Form validation: no shares
        if not request.form.get("shares"):
            return apology("must provide number of shares", 400)
        # Form validation: shares must be a positive int
        shares = int(request.form.get("shares"))
        if shares <= 0:
            return apology("must provide valid number of shares", 400)
        # Form validation: user must have that number of shares.
        symbol = lookup(symbol)["symbol"]
        usershares = db.execute("SELECT shares FROM portfolios WHERE user_id = :user_id AND symbol = :symbol",
                                user_id=session["user_id"], symbol=symbol)
        if len(usershares) != 1:
            return apology("you don't have shares of that company", 400)

        usershares = usershares[0]["shares"]

        if shares > usershares:
            return apology("you don't have that many shares", 400)

        # Get the total cash the shares are valued at, and the cash the user has:
        price = lookup(symbol)["price"]
        company = lookup(symbol)["name"]

        total = price * shares
        usercash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])
        usercash = usercash[0]["cash"]

        usercash_after = usercash + total

        # Now that the sell submit is validated, we must execute the transaction and update the corresponding tables. Insert a new transaction:
        db.execute("INSERT INTO transactions (transaction_type, user_id, symbol, stock_price, shares, total_transaction, cash_previous, cash_after) VALUES ('SELL', :user_id, :symbol, :stock_price, :shares, :total_transaction, :cash_previous, :cash_after)",
                   user_id=session["user_id"], symbol=symbol, stock_price=price, shares=shares, total_transaction=total, cash_previous=usercash, cash_after=usercash_after)

        # Update the shares value in portfolios.
        updatedshares = usershares - shares
        db.execute("UPDATE portfolios SET shares = :updatedshares WHERE user_id=:user_id AND symbol =:symbol",
                   updatedshares=updatedshares, user_id=session["user_id"], symbol=symbol)

        # Update user remaining cash:
        db.execute("UPDATE users SET cash = :cash_after WHERE id = :user_id", cash_after=usercash_after, user_id=session["user_id"])

        # Cleanup function
        db.execute("DELETE FROM portfolios WHERE user_id=:user_id AND shares = 0", user_id=session["user_id"])

        return redirect("/")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
