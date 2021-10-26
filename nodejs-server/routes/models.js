var express = require('express');


var router = express.Router();

/* GET  listing. */
router.get('/', function(req, res, next) {
  res.send('respond with a resource');
});


/* Download model */
router.get('/download/:cnt', (req, res) => {
  res.send('get download request');

  cnt_name = req.params.cnt;
  console.log(typeof(cnt_name))
  console.log(cnt_name)
  
  //  res.send(`req for ${cnt_name}`)
}); 



router.post('/', function(req, res, next){
    res.send('post request')
});



router.delete('/', function(req, res, next){
    res.send('delete request')
});




module.exports = router;
