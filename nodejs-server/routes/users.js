var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
  res.send('respond with a resource');
});

router.post('/', function(req, res, next) {
  res.send(req.body);

  if(!res.req.body)
    return res.status(400).json({message: 'req body cannot be empty'})
});

module.exports = router;


