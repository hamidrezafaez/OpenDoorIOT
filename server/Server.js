  var express = require('express');
    var favicon = require('serve-favicon');
    var logger = require('morgan');
    var methodOverride = require('method-override');
    var session = require('express-session');
    var bodyParser = require('body-parser');
    var multer = require('multer');
    var errorHandler = require('errorhandler');
 var http = require('http');
 var path = require('path');

 var app = express();

 // all environments
   app.set('port', process.env.PORT || 3000);
    app.set('views', __dirname + '/views');
    app.set('view engine', 'jade');
    //app.use(favicon(__dirname + '/public/favicon.ico'));
    app.use(logger('dev'));
    app.use(methodOverride());
    app.use(session({ resave: true, saveUninitialized: true, 
                      secret: 'uwotm8' }));

    // parse application/json
    app.use(bodyParser.json());                        

    // parse application/x-www-form-urlencoded
    app.use(bodyParser.urlencoded({ extended: true }));

    // parse multipart/form-data
    app.use(multer());

    app.use(express.static(path.join(__dirname, 'public')));

 var net = require('net');

 var sockets = [];
 var socketdic = [];
 var mainsocket;

 var humidity = null;
 var soil_moisture= null;
 var temprture = null;

 function receiveData(data,socket) {
     // console.log(String.fromCharCode(data[0]));
     // console.log(data.length);
     // console.log(data);
     try {
         var sens = "";
         for (var i = 0; i < data.length; i++) {
             sens += String.fromCharCode(data[i]);
         }
         console.log(sens);
         var jsonres = JSON.parse(sens);
         console.log(jsonres.status);
         if(jsonres.SN != undefined){
            //var mysocket = sockets.find(s => socket.remotePort == socket.remotePort);
            socketdic.push({SN:jsonres.SN,port:socket.remotePort});
            console.log(jsonres.SN);
            //console.log(mysocket)
         }
         
		 /*
         sens = sens.split(" ");
         var stringArray = new Array();
         for (var i = 0; i < sens.length; i++) {
             stringArray.push(sens[i]);
             if (i != sens.length - 1) {
                 stringArray.push(" ");
             }
         }
         temprture = sens[0];
         soil_moisture = sens[1];
         humidity = sens[2];
		 */
        socket.write("OK");
     }
     catch (ex){
         console.log(ex);
     }
     // for(var i = 0; i<sockets.length; i++) {
     //     sockets[i].write(data);
     // }
 }
//-------------------------------------------------------
 function closeSocket(socket) {
     var i = sockets.indexOf(socket);
     if (i != -1) {
         sockets.splice(i, 1);
         socketdic.splice(i, 1);
     }
     console.log("Arduino get OUT!!!");
 }
//---------------------------------------------------------
 app.get('/on', function(req, res){
     mainsocket.write("moto");
     res.json({Motor:"ON"});
 });

 app.get('/off', function(req, res){
     mainsocket.write("off");
     res.json({Motor:"OFF"});
 });

 app.get('/a', function(req, res){
//     mainsocket.write("a");
     res.json({name:"hamid"});
 });

 app.get('/on/:SN',function(req,res){
     var socketSN = socketdic.find(s => s.SN == req.params.SN);
    var mysocket = sockets.find(s => s.remotePort == socketSN.port);
    mysocket.write("moto");
    console.log(`Arduino with SN = `+ req.params.SN +` Open!!!`);
    res.json({Motor:String(req.params.SN)});
 });

 app.get('/b', function(req, res){
     mainsocket.write("send to client\n");
     res.json({
         Temperature:temprture,
         Soil_moisture:soil_moisture,
         Humidity:humidity
     });
 });
 // app.put('/flight/:number/arrived', routes.arrived);
 //-------------------------------------------------------

 function newSocket(socket) {
     //let socketNumber = sockets.length();
     mainsocket = socket;
     sockets.push(socket);
     console.log("new Arduino Connected");
     socket.write('Welcome to Telnet HRF!\n');
	 console.log("new Arduino login");
     socket.on('data', function(data) {
         receiveData(data,socket);
     })
     socket.on('end', function() {
         closeSocket(socket);
     })
     socket.on('error', function(err) {
         console.log(err);
         console.log("\n Error \n");
     });
 }

 // Create a new server and provide a callback for when a connection occurs
 var server = net.createServer(newSocket);

 // Listen on port 2000
 server.listen(2000,function () {
     console.log('Tellnet server listening on port 2000');
 });

 http.createServer(app).listen(app.get('port'), function(){
     console.log('Express server listening on port ' + app.get('port'));
 });
