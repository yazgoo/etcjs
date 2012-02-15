ETCJS_ERROR = 0
ETCJS_LIST = 1
ETCJS_CONTENT = 2
function Owner()
{
    this.create = function(callback)
    {
        this.etcjs.post("owner/create", ETCJS_CONTENT)
    }
}
function Configuration()
{
    this.post = function(callback, what, array)
    {
        what = what || ETCJS_CONTENT
        array = array || new Array()
        this.etcjs.post(callback,
                "config/" + arguments.callee.caller.name, what, array)
    }
    this.set = function(content, callback, offset, size)
    {
        offset = offset || null
        size = size || 0
        if(offset == null)
            return this.post(callback, ETCJS_CONTENT, {"content":content})
        else return this.post(callback, ETCJS_CONTENT,
                {"content":content, "offset":offset, "size":size})
    }
    this.touch = function touch(callback) { this.post(callback); }
    this.delete_ = function delete_(callback) { this.post(callback); }
    this.get = function get(callback) { this.post(callback); }
    this.stat = function stat(callback) { this.post(callback); }
    this.list = function list(callback)
    {
        this.etcjs.post(
                function(callack)
                {
                    if(result.type != EtcjsERROR)
                    result.result = explode("\n", result.result)
                    callback(result)
                }, "config/list", ETCJS_LIST, new Array())
    }
}
function Server(host_name, port)
{
    this.host_name = host_name
    this.port = port
}
function Result(type, result)
{
    this.type = type
    this.result = result
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
    this.post = function(callback, path, type, array)
    {
        console.log(path + " | " + type + " | " + array)
    }
}
var etcjs = new Etcjs(new Owner("yazgoo", "foo"), new Server("localhost", 1337))
var config = etcjs.get_configuration("blah")
config.get(function(result)
        {
            console.log(result)
        })
