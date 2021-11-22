const express = require('express');
const path = require('path');
const multer = require('multer');
const conf = require('./conf')

var indexRouter = require('./routes/index');
var modelsRouter = require('./routes/models');

const upload = multer();

var app = express();

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));
app.use(express.urlencoded({ extended: true }));
app.use(upload.any())

app.use('/', indexRouter);
app.use('/models', modelsRouter);


// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});


// server "localhost:3000
port = conf.port
app.listen(port, () =>{
  console.log('localhost:', port)
  console.log('app listening on port ', port)
})