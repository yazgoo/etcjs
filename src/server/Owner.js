var Paths = require('./Paths.js').Paths
var fs = require('fs')
var crypto = require('crypto')
function Owner(name, key)
{
    var self = this
    this.name = name
    var crypto = require('crypto')
      , shasum = crypto.createHash('sha1')
    shasum.update(key)
    this.key = shasum.digest('hex')
    this.paths = new Paths().get(this.name)
    this.callback
    this.create_key = function(callback)
    {
        fs.writeFile(self.paths.key_path, self.key, callback)
    }
    this.create = function(callback)
    {
        self.callback = callback
        self.exists(function(callback)
        {
            return fs.mkdir(self.paths.full_path, 0755,
                function(err)
                {
                    self.create_key()
                    self.callback(true)
                })
        })
    }
    this.exists = function(callback)
    {
        dirs = fs.readdir(new Paths().root, function(err, dirs)
                {
                    for(i in dirs)
                    {
                        if(dirs[i] == self.paths.dir_name) callback(true)
                    }
                    callback(false)
                })
    }
    this.validates = function(callback)
    {
        fs.readFile(this.paths.key_path, null,
               function(err, data) { callback(err, data == self.key) })
    }
}
module.exports.Owner = Owner
