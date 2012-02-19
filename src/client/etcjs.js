ETCJS_ERROR = 0
ETCJS_LIST = 1
ETCJS_CONTENT = 2
var fs = require('fs')
function Owner(name, key)
{
    this.name = name
    this.key = key
    this.create = function(callback)
    {
        this.etcjs.post("owner/create", ETCJS_CONTENT)
    }
}
function Configuration(name)
{
    this.name = name
    this.post = function(callback, what, array)
    {
        what = what || ETCJS_CONTENT
        array = array || {}
        array["name"] = this.name
        this.etcjs.post(callback,
                "config/" + arguments.callee.caller.name, what, array)
    }
    this.set = function set(content, callback, offset, size)
    {
        offset = offset || null
        size = size || 0
        if(offset == null)
            return this.post(callback, ETCJS_CONTENT, {"content":content})
        else return this.post(callback, ETCJS_CONTENT,
                {"content":content, "offset":offset, "size":size})
    }
    this.touch = function touch(callback) { this.post(callback); }
    this.remove = function remove(callback) { this.post(callback); }
    this.get = function get(callback) { this.post(callback); }
    this.stat = function stat(callback) { this.post(callback); }
    this.list = function list(callback)
    {
        this.etcjs.post(
                function(result)
                {
                    if(result.type != ETCJS_ERROR)
                    result.result = result.result.split("\n")
                    callback(result)
                }, "config/list", ETCJS_LIST, new Array())
    }
}
function Server(host_name, port)
{
    this.host_name = host_name
    this.port = port
}
/*
   This might be used instead of Server if you want to query a file on the
   same domain/port (I use it with Apache httpd reverse proxy, because of
   the cross site limitations of XMLHttpRequest).
   This behaviour is only available with XMLHttpRequest client, not with node.js
   */
function LocalPath(path)
{
    this.path = path
}
function Result(type, result)
{
    this.type = type
    this.result = result
}
function ServerFactory(suffix)
{
    suffix = suffix || ""
    content = fs.readFileSync(
            process.env.HOME + "/.config/etcjs_cli/server" + suffix, "utf-8")
    data = content.split(":")
    return new Server(data[0], data[1])
}
function Etcjs(owner, server)
{
    this.owner = owner
    this.server = server
    this.owner.etcjs = this
    this.get_configuration = function(name)
    {
        var config = new Configuration(name)
        config.etcjs = this
        return config
    }
    this.encode_options = function(options)
    {
        var result = ""
        var i = 0
        for(key in options)
        {
            if(i > 0) result += "&"
            result += key + "=" + options[key]
            i++
        }
        return result
    }
    this.xml_http_post = function(callback, path, type, options)
    {
        var uri = self.server.path == undefined?
            "http://" + self.server.host_name + ":" + self.server.port
            : self.server.path + "/" + path 
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open("POST", uri)
        xmlhttp.onreadystatechange = function()
        {
            if (xmlhttp.readyState == 4)
            {
                callback(xmlhttp.status == 200?type:ETCJS_ERROR,
                        xmlhttp.responseText)
            }
        }
        xmlhttp.send(self.encode_options(options))
    }
    self = this
    this.nodejs_post = function(callback, path, type, options)
    {
        var client = require('http')
            .createClient(self.server.port, self.server.host_name)
        var params = self.encode_options(options)
        var request = client.request("GET", "/" + path, {
                        "Content-Length": "" + params.length})
        var result = ""
        request.write(params)
        request.end();
        request.on('response', function(response)
        {
            var status = response.statusCode
            response.on('data', function(data) { result += data })
            response.on('end', function()
                {
                    callback(new Result(status == 200?type:ETCJS_ERROR, result))
                })
        })
    }
    this.post = function(callback, path, type, options)
    {
        options.owner = this.owner.name
        options.key = this.owner.key
        var f = typeof XMLHttpRequest == 'function'?
            this.xml_http_post:this.nodejs_post
        f(callback, path, type, options)
    }
}
module.exports.Etcjs = Etcjs
module.exports.Configuration = Configuration
module.exports.Server = Server
module.exports.ServerFactory = ServerFactory
module.exports.LocalPath = LocalPath
