var server = require("./server")
var router = require("./router")
var args = process.argv
if(args.length < 4)
{
    console.log("Usage: " + args[0] + " " + args[1] + " port store [driver]")
    console.log(" port\tthe port etcjs will listen to")
    console.log(" store\tthe path to the store directory")
    console.log(" driver\tName of the driver that will provide data access")
    console.log("\t\tempty: filesystem serialization")
    console.log("\t\tproxy: proxy serialization")
    return
}
var port = parseInt(args[2])
var store_path = args[3]
driver = args[4]
var request_handler = require("./request_handler")
paths = require('./Paths.js').root = store_path
server.start(port, router.route, {
    "/owner/create" : request_handler.create_owner,
    "/config/get"   : request_handler.get_config,
    "/config/set"   : request_handler.set_config,
    "/config/delete": request_handler.delete_config,
    "/config/touch" : request_handler.touch_config,
    "/config/list"  : request_handler.list_config,
    "/config/stat"  : request_handler.stat_config,
})
