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
        self.get_callback = callback
        fs.readFile(this.owner.paths.config(name), null, 
               function(err, data) { callback(data) })
    }
}
module.exports.Config = Config
