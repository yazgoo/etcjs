var fs = require('fs')
var Owner = require('./Owner.js').Owner
var Config = require('./Config.js').Config
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
