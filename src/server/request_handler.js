var querystring = require('querystring')
var sys = require('sys')
String.prototype.capitalize = function()
{
    if(this.length == 0) return this;
    return this.charAt(0).toUpperCase() + this.slice(1);
}
driver = driver || ""
driver = driver.capitalize()
var Owner = require('./Owner' + driver).Owner
var Config = require('./Config' + driver).Config
function write_header(response, ok)
{
    response.writeHead(ok?200:401,
            {"Content-Type": "text/plain"})
}
function create_owner(response, post)
{
    console.log("Request handler 'create_owner' was called.")
    post = querystring.parse(post)
    new Owner(post.name, post.key).create(function(ok)
    {
        write_header(response, ok)
        response.write("creating " + post.name + " " +
            (ok?"succeed":"fail") + "ed.\n")
        response.end()
    })
}
function on_config(response, post, callback)
{
    post = querystring.parse(post)
    owner = new Owner(post.owner, post.key)
    owner.validates(function(err, validates)
    {
        write_header(response, validates)
        if(!validates)
        {
            response.write("does not validate " + err + " " +validates)
            response.end()
        }
        else
        {
            var config = new Config(owner, post.name)
            callback(config, post)
        }
    })
}
function set_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        if(post.content == null) 
        {       
            write_header(response, false)
            response.write("missing content")
            response.end()
            return
        }
        console.log("setting config " + post.content)
        config.set(post.offset, post.size, post.content, function(err)
        {
            error_handling_write_parameter(response, err, "creating succeeded\n")
        })
    })
}
function touch_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.touch(function(err)
        {
            error_handling_write_parameter(response, err, "touch succeeded\n")
        })
    })
}
function delete_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.remove(function(err)
        {
            error_handling_write_parameter(response, err, "remove succeeded\n")
        })
    })
}
function get_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.get(post.offset, post.size, function(data)
        {
            error_handling_write_parameter(response, data == null?"no data found":null, data)
        })
    })
}
function error_handling_write_parameter(resp, err, parameter)
{
    error_handling_write(resp, err, parameter, function(parameter)
    {
        resp.write(parameter)
    })
}
function error_handling_write(resp, err, parameter, callback)
{
    write_header(resp, err == null)
    if(err == null) callback(parameter)
    else resp.write(err + "\n")
    resp.end()
}
function list_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.list(function(err, names)
        {
            error_handling_write(response, err, names, function(names)
            {
                for(i in names) response.write(names[i] + "\n")
            })
        })
    })
}
function stat_config(response, post)
{
    on_config(response, post, function(config, post)
    {
        config.stat(function(err, stats)
        {
            error_handling_write(response, err, stats, function(stats)
            {
                for(key in stats)
                    if(typeof stats[key] != 'function')
                        response.write(key + "=" + stats[key] + "\n")
            })
        })
    })
}
exports.create_owner = create_owner
exports.set_config = set_config
exports.get_config = get_config
exports.touch_config = touch_config
exports.list_config = list_config
exports.delete_config = delete_config
exports.stat_config = stat_config
