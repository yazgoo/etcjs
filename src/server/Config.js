var Paths = require('./Paths.js').Paths
var fs = require('fs')
var k = require('constants')
function Config(owner, name)
{
    this.owner = owner
    this.name = name;
    var self = this
    this.set = function(offset, size, data, callback)
    {
        var path = this.owner.paths.config(name);
        if(offset == null || size == null) fs.writeFile(path, data, callback)
        else
        {
            data = new Buffer(data, 'utf8')
            self.data = data
            fs.open(path, k.O_WRONLY, 0666, function(err, fd)
                    {
                        console.log("offest: " + offset)
                        if(err != null) callback(null)
                        else fs.write(fd, data, 0, parseInt(size), parseInt(offset), callback);
                    })
        }
    }
    this.get = function(offset, size, callback)
    {
        fs.readFile(this.owner.paths.config(name), null, 
               function(err, data) { callback(data) })
    }
    this.touch = function(callback)
    {
        fs.writeFile(this.owner.paths.config(name), "",
               function(err, data) { callback(data) })
    }
    this.remove = function(callback)
    {
        fs.unlink(this.owner.paths.config(name),
               function(err) { callback(err) })
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
    this.stat = function(callback)
    {
        fs.stat(this.owner.paths.config(name),
               function(err, stats) { callback(err, stats) })
    }
}
module.exports.Config = Config
