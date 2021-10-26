var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

const multer = require("multer");
const mongodb = require('mongodb');
const MongoClient = require('mongodb').MongoClient;
const ObjectID = require('mongodb').ObjectID;
var fs = require("fs");
var assert = require('assert');


var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
var modelsRouter = require('./routes/models');


var app = express();


// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use(multer().array())

app.use('/', indexRouter);
app.use('/users', usersRouter);
app.use('/models', modelsRouter);



const mongoURI = 'mongodb://localhost:27017/test'   // db
let db;
MongoClient.connect(mongoURI, (err, client) => {
  assert.ifError(err)
  db = client.db('test')
  // var bucket = new mongodb.GridFSBucket(db)
});


// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});


module.exports = app;


// server "localhost:3000"

port = 3000
app.listen(port, () =>{
  console.log('localhost:', port)
  console.log('app listening on port ', port)
})