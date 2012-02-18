function Owner(name, key)
{
    this.name = name
    this.key = key
    this.create = function(callback)
    {
    }
    this.exists = function(callback)
    {
        return true
    }
    this.validates = function(callback)
    {
        callback("", true)
    }
}
module.exports.Owner = Owner
