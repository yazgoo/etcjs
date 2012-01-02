var root
function log()
{
    console.log(root)
}
function Paths()
{
    this.root = root
    if(this.root == null) this.root = "../../store"
    self = this
    this.escape_special_chars = function(string)
    {
        return string
    }
    this.get = function(name)
    {
        this.dir_name = self.escape_special_chars(name)
        this.full_path = new Paths().root + "/" + this.dir_name
        this.key_path = this.full_path + "/key"
        this.config = function(name)
        {
            return this.full_path + "/" + self.escape_special_chars(name)
        }
        return this
    }
}
module.exports.root = root
module.exports.Paths = Paths
