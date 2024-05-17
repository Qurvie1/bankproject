#!/usr/bin/env python
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request, redirect, url_for, flash
from flask_login import LoginManager, UserMixin, login_user, current_user, logout_user
import socket

app = Flask(__name__)
app.secret_key = 'rofls'
login_manager = LoginManager()
login_manager.init_app(app)
sock = socket.socket()
initialized = False

class User(UserMixin):
    def __init__(self, used_id, login, password, name, surname, address, series, number, acc1, acc2, acc3) -> None:
        self.id = used_id
        self.login = login
        self.password = password
        self.name = name
        self.surname = surname
        self.address = address
        self.series = series
        self.number = number
        self.acc1 = acc1
        self.acc2 = acc2
        self.acc3 = acc3

users = {}

def add_user(user_id, login, password):
    new_user = User(user_id, login, password)
    users[user_id] = new_user

def connection():
    global initialized
    if not initialized:
        sock.connect(("127.0.0.1", 9084))
        initialized = True

@app.before_request
def checker():
    connection()

@login_manager.user_loader
def load_user(user_id):
    return users.get(user_id)

@app.route('/')
def index():
    if current_user.is_authenticated:
        return redirect(url_for('cabinet'))
    else:
        return render_template('index.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if current_user.is_authenticated:
        return redirect(url_for('cabinet'))
    if request.method == 'POST':
        username = request.form['username']
        passw = request.form['password']
        print('GET', username, passw)
        message = '1 ' + username + ' ' + passw
        sock.send(message.encode())
        data = sock.recv(1024).decode()
        if data[0] == '1':
            array = data.split()
            user = User(array[1], username, passw, array[2], array[3], array[4], array[5], array[6], array[7], array[8], array[9])
            users[array[1]] = user 
            login_user(user)
            return redirect(url_for('cabinet'))
        else:
            flash('Invalid login or password')
    return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if current_user.is_authenticated:
        return(redirect(url_for('cabinet')))
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        name = request.form['name']
        surname = request.form['surname']
        address = request.form['address']
        series = request.form['series']
        number = request.form['number']
        message = '2 ' + name + ' ' + surname + ' ' + address + ' ' + series + ' ' + number + ' ' + username + ' ' + password
        sock.send(message.encode())
        data = sock.recv(1024).decode()
        if data[0] == '1':
            flash('Account created successfully')
            return redirect(url_for('login'))
        else:
            flash('This login and password already exists')
    return render_template('register.html')


@app.route('/cabinet', methods=['GET', 'POST'])
def cabinet():
    if not current_user.is_authenticated:
        return redirect(url_for('login'))
    if request.method == 'POST':
        action = request.form['action']

        if action == 'action1':
            return redirect(url_for('transaction'))
        if action == 'action2':
            return redirect(url_for('addm'))
        if action == 'action3':
            return redirect(url_for('opener'))

    return render_template('cabinet.html', user=current_user)

@app.route('/transaction', methods=['GET', 'POST'])
def transaction():
    if not current_user.is_authenticated:
        return redirect(url_for('login'))
    if request.method == 'POST':
        login1 = request.form['username1']
        login2 = request.form['username2']
        amount = request.form['amount']

        message = '5 ' + login1 + ' ' + login2 + ' ' + amount + ' '
        message += current_user.id
        sock.send(message.encode())
        datat = sock.recv(1024).decode()
        if datat[0] == '1':
            flash('Transaction succesful')
            return redirect(url_for('cabinet'))
        else:
            flash('Transaction failed')
    return render_template('transaction.html')

@app.route('/addm', methods=['GET', 'POST'])
def addm():
    if not current_user.is_authenticated:
        return redirect(url_for('login'))
    if request.method == 'POST':
        amount = request.form['amount']
        account = request.form['account']
        action = request.form['action']
        message = ""
        if action == 'add':
            message += '4 '
        elif action == 'borrow':
            message += '6 '
        if account == 'Дебетовой':
            message += '0 '
        elif account == 'Кредитный':
            message += '2 '
        elif account == 'Депозит':
            message += '1 '
        message += amount + ' '
        message += current_user.id
        print(message)
        sock.send(message.encode())
        datat = sock.recv(1024).decode()
        if datat[0] == '1':
            flash('Changing succesful')
            user = users[current_user.id]
            if action == 'add':
                if account == 'Дебетовой':
                    user.acc1 += amount
                elif account == 'Депозит':
                    user.acc2 += amount
                elif account == 'Кредитный':
                    user.acc3 += amount
            elif action == 'borrow':
                if account == 'Дебетовой':
                    user.acc1 -= amount
                elif account == 'Депозит':
                    user.acc2 -= amount
                elif account == 'Кредитный':
                    user.acc3 -= amount
            return redirect(url_for('cabinet'))
        else:
            flash('You havent enough money')
    return render_template('addm.html')

@app.route('/opener', methods=['GET', 'POST'])
def opener():
    if not current_user.is_authenticated:
        return redirect(url_for('login'))
    if request.method == 'POST':
        action = request.form['action']
        message = ""
        if action == 'act1':
            message += '0 '
        elif action == 'act2':
            message += '1 '
        elif action == 'act3':
            message += '2 '
        message += current_user.id
        print(message)
        sock.send(message.encode())
        datat = sock.recv(1024).decode()
        if datat[0] == '1':
            user = users[current_user.id]
            flash('Opening succesful')
            if action == 'act1':
                user.acc1 = 0
            if action == 'act2':
                user.acc2 = 0
            if action == 'act3':
                user.acc3 = 0
            return redirect(url_for('cabinet'))
        else:
            flash('You can not open account')
    return render_template('opener.html')

@app.route('/logout')
def logout():
    logout_user()
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(debug=True)
    sock.close()