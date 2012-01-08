var fs = require('fs')
path = "../src/server/"
var Owner = require(path + 'Owner.js').Owner
var Config = require(path + 'Config.js').Config
paths = require(path + 'Paths.js').root = process.argv[1]
function print(x)
{
    process['stdout'].write('' + x + '\n')
}
var owner = new Owner("laurentmaury", "key")
owner.create(function(ok)
{
    owner.validates(function(err, validates)
    {
        if(!validates) throw "does not validate " + err + " " +validates
        var config = new Config(owner, "main")
        config.set("lol", function(err) { print(err) })
        config.get(function(data) { print(data) })
    })
})
