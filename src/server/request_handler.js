var querystring = require('querystring')
var Owner = require('./Owner').Owner
var Config = require('./Config').Config
function create_owner(response, post)
{
    console.log("Request handler 'create_owner' was called.")
    response.writeHead(200, {"Content-Type": "text/plain"})
    post = querystring.parse(post)
    new Owner(post.name, post.key).create(function(ok)
    {
        response.write("creating " + post.name + " " +
            (ok?"succeed":"fail") + "ed.\n")
        response.end()
    })
}
function on_config(response, post, callback)
{
    post = querystring.parse(post)
    owner = new Owner(post.owner, post.key)
    response.writeHead(200, {"Content-Type": "text/plain"})
    owner.validates(function(err, validates)
    {
        if(!validates) throw "does not validate " + err + " " +validates
        var config = new Config(owner, post.name)
        callback(config, post)
    })
}
function set_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        if(post.content == null) 
        {       
            response.end()
            return
        }
        console.log("creating config " + post.content)
        config.set(post.content, function(err)
        {
            response.write("creating " + post.name + " " + (err != null)+ " " +
                ((err == null)?"succe":"fail") + "ed.\n")
            response.end()
        })
    })
}
function get_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.get(function(data)
        {
            response.write(data)
            response.end()
        })
    })
}
function list_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.list(function(err, names)
        {
            if(err == null)
                for(i in names) response.write(names[i] + "\n")
            response.end()
        })
    })
}
exports.create_owner = create_owner
exports.set_config = set_config
exports.get_config = get_config
exports.list_config = list_config
