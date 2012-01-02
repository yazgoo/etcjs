var http = require("http");
var url = require("url");
function start(port, route, handle)
{
  function onRequest(request, response)
  {
    var post = ""
    var pathname = url.parse(request.url).pathname
    console.log("Request for " + pathname + " received.")
    request.setEncoding("utf8")
    request.addListener("data", function(postChunk)
    {
      post += postChunk
      console.log("Received POST data chunk '"+
      postChunk + "'.")
    })
    request.addListener("end", function()
    {
      route(handle, pathname, response, post)
    })
  }
  http.createServer(onRequest).listen(port)
  console.log("Server has started.")
}
exports.start = start;
