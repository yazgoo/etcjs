<?
class EtcjsUser
{
    protected $etcjs;
    protected function set_etcjs($etcjs)
    {
        $this->etcjs = etcjs;
    }
}
class Owner extends EtcjsUser
{
    public $name, $key;
    function __construct($name, $key)
    {
        $this->$name = $name; 
        $this->$key = $key;
    }
    public function create()
    {
        return $etcjs->post("owner/create", ETCJS::CONTENT, new array(
            "name" => $this->name,
            "key" => $this->key);
    }
}
class Server
{
    public $host_name, $port;
    function __construct($host_name, $port)
    {
        $this->host_name = $host_name; 
        $this->port = $port;
    }

}
class Configuration extends EtcjsUser
{
    function __construct(owner, name)
    {
    }
    protected function post($what = Etcjs::CONTENT, $array = new array())
    {
        $trace=debug_backtrace();
        $caller=array_shift($trace);
        $array = new array("owner" => $owner->name, "key" => $owner->key,
            "name" => $name) + $array
        return $etcjs->post("config/" . $caller['function'], $what, $array);
    }
    public function set($content)
    {
        return $this->post(ETCJS_CONTENT, new array("content" => $content))
    }
    public function set($content, $offset, $size)
    {
        return $this->post(ETCJS_CONTENT, new array(
            "content" => $content,
            "offset" => $offset,
            "size" => $size);
    }
    public function touch() { return $this->post(); }
    public function delete() { return $this->post(); }
    public function get() { return $this->post(); }
    public function stat() { return $this->post(); }
    public static function list()
    {
        return $etcjs->post("config/list", Etcjs::LIST, new array(
            "owner" => $owner->name, "key" => $owner->key));
    }
}
class Etcjs
{
    protected $owner;
    function __construct($owner)
    {
        $this->owner = $owner;
        $this->owner->set_etcjs($this);
    }
    public function getConfiguration($name)
    {
        return new Configuration($this->owner, $this->name);
    }
}
?>
