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
        this.remote_config.set(data, 
            function(result)
            {
                if(result.type == ETCJS_ERROR)
                    callback(result.result, null)
                else callback(null, size)
            } , offset, size)
    }
    this.get = function(offset, size, callback)
    {
        this.on_remote_config("get", callback, null, true)
    }
    this.touch = function(callback)
    {
        this.on_remote_config("touch", callback, null, true)
    }
    this.remove = function(callback)
    {
        this.remote_config.remove(function(result)
            {
                if(result.type == ETCJS_ERROR) callback(result.result)
                else callback(null)
            })
    }
    this.identity = function(x)
    {
        return x
    }
    this.on_remote_config = function(method, callback, translater, single_param)
    {
        translater = translater || null
        single_param = single_param || false
        if(translater == null) translater = this.identity
        this.remote_config[method](function(result) {

                    if(single_param)
                         callback(translater(result.result))
                    else
                    {
                        if(result.type == ETCJS_ERROR)
                            callback(result.result, null)
                        else
                            callback(null, translater(result.result))
                    }
                })
    }
    this.list = function(callback)
    {
        this.on_remote_config("list", callback)
    }
    this.stat_string_to_map = function(string)
    {
        var lines = string.split("\n")
        var map = {}
        for(i in lines)
        {
            var item = lines[i].split("=")
            if(item.length < 2) continue
            map[item[0]] = item[1]
        }
        return map
    }
    this.stat = function(callback)
    {
        this.on_remote_config("stat", callback, this.stat_string_to_map)
    }
}
module.exports.Config = Config
