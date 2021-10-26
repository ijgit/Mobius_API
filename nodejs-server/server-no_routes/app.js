/*
19.09.22
add download
add file delete
*/

const express = require("express");
const router = express.Router();
const multer = require("multer");
var request = require("request");
const mongodb = require("mongodb");
const MongoClient = require("mongodb").MongoClient;
const ObjectID = require("mongodb").ObjectID;
const bodyParser = require("body-parser");
var gridfs = require("gridfs-stream");
var fs = require("fs");
var assert = require("assert");

var addr = "http://192.168.0.1:3000/models/";

/*
  NodeJS Module dependencies.
*/
const { Readable } = require("stream");

/*
  Create Express server && Express Router configuration.
*/
const app = express();
app.use(bodyParser.json());
app.use("/models", router);
/*
  Connect Mongo Driver to MongoDB.
*/
let db;
MongoClient.connect("mongodb://localhost:27017", (_err, client) => {
  db = client.db("test");
});



/*
  POST /images
*/
router.post("/", (req, res) => {
  let cnt = req.body.cnt;
  let ae = req.body.ae;

  console.log(cnt);
  console.log(ae);


  const storage = multer.memoryStorage();
  const upload = multer({ storage: storage }); //, limits: { fields: 2, files: 1, parts: 3 }

  // need to key name = file
  upload.single("file")(req, res, (err) => {
    if (err) {
       returnres
        .status(400)
        .json({ message: "Upload Request Validation Failed" });
    } else if (!req.body.cnt) {
      return res.status(400).json({ message: "No image name in request body" });
    }

    // Covert buffer to Readable Stream
    const readableTrackStream = new Readable();
    readableTrackStream.push(req.file.buffer);
    readableTrackStream.push(null);

    let bucket = new mongodb.GridFSBucket(db, {
      bucketName: cnt, // change to req.body.user
    });

    let uploadStream = bucket.openUploadStream(cnt);
    let id = uploadStream.id;

    var img_rsc = cnt + "?obj=" + id;
    var img_url = addr + img_rsc;

    readableTrackStream.pipe(uploadStream);
    uploadStream.on("error", () => {
      return res.status(500).json({ message: "Error uploading file" });
    });

    uploadStream.on("finish", () => {
      console.log(id);
      console.log(img_url)

      return res
        .status(201)
        .json({
          message:
            "File uploaded successfully, stored under Mongo ObjectID: " + id,
        });
    });
  });
});

app.listen(3000, () => {
  console.log("App listening on port 3000!");
});
