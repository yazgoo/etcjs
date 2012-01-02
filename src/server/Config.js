var Paths = require('./Paths.js').Paths
var fs = require('fs')
function Config(owner, name)
{
    this.owner = owner
    this.name = name;
    var self = this
    this.set = function(data, callback)
    {
        self.data = data
        fs.writeFile(this.owner.paths.config(name), data, callback)
    }
    this.get = function(callback)
    {
        fs.readFile(this.owner.paths.config(name), null, 
               function(err, data) { callback(data) })
    }
    this.list = function(callback)
    {
        fs.readdir(this.owner.paths.full_path, function(err, files)
        {
            var result = []
            var j = 0
            if(err == null) for(i in files) if(files[i] != "key")
                result[j++] = this.owner.paths.unescape_special_chars(files[i])
            callback(err, result)
        })
    }
}
module.exports.Config = Config
