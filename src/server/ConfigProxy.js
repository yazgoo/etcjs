var client = require('../client/etcjs')
function Config(owner, name)
{
    this.owner = owner
    this.name = name;
    this.server = null
    this.client =
    new client.Etcjs(this.owner, client.ServerFactory("_proxyfied"))
    this.remote_config = this.client.get_configuration(name)
    var self = this
    this.set = function(offset, size, data, callback)
    {
        this.remote_config.set(data, offset, size, callback)
    }
    this.get = function(offset, size, callback)
    {
        this.remote_config.get(callback)
    }
    this.touch = function(callback)
    {
        this.remote_config.touch(callback)
    }
    this.remove = function(callback)
    {
        this.remote_config.remove(callback)
    }
    this.list = function(callback)
    {
        this.remote_config.list(function(result)
                {
                    if(result.type == ETCJS_ERROR)
                        callback(result.result, null)
                    else callback(null, result.result)
                })
    }
    this.stat = function(callback)
    {
        this.remote_config.stat(function(result)
                {
                    if(result.type == ETCJS_ERROR)
                    {
                        callback(result.result, null)
                    }
                    else
                    {
                        var lines = result.result.split("\n")
                        var map = {}
                        for(i in lines)
                        {
                            var item = lines[i].split("=")
                            map[item[0]] = item[1]
                        }
                        callback(null, map)
                    }
                })
    }
}
module.exports.Config = Config
