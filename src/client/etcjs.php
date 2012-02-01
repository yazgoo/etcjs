<?
class EtcjsUser
{
    protected $etcjs;
    public function set_etcjs($etcjs)
    {
        $this->etcjs = $etcjs;
    }
}
class Owner extends EtcjsUser
{
    public $name, $key;
    function __construct($name, $key)
    {
        $this->name = $name; 
        $this->key = $key;
    }
    public function create()
    {
        return $etcjs->post("owner/create", ETCJS::CONTENT);
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
    protected $name;
    function __construct($name)
    {
        $this->name = $name;
    }
    protected function post($what = Etcjs::CONTENT, $array = array())
    {
        $trace = debug_backtrace();
        $caller = array_shift($trace);
        $caller = $trace[0]['function'];
        $array = array("name" => $this->name) + $array;
        return $this->etcjs->post("config/" . $caller, $what, $array);
    }
    public function set($content, $offset = null, $size = 0)
    {
        if($offset == null)
            return $this->post(Etcjs::CONTENT, array("content" => $content));
        else return $this->post(Etcjs::CONTENT, array(
            "content" => $content,
            "offset" => $offset,
            "size" => $size));
    }
    public function touch() { return $this->post(); }
    public function delete() { return $this->post(); }
    public function get() { return $this->post(); }
    public function stat() { return $this->post(); }
    public function list_()
    {
        $result = $this->etcjs->post("config/list", Etcjs::LIST_, array());
        if($result->type != Etcjs::ERROR)
            $result->result = explode("\n", $result->result);
        return $result;
    }
}
class Result
{
    public $type, $result;
    function __construct($type, $result)
    {
        $this->type = $type;
        $this->result = $result;
    }
}
class Etcjs
{
    const ERROR = 0;
    const LIST_ = 1;
    const CONTENT = 2;
    protected $owner, $sever, $handle;
    function __construct($owner, $server)
    {
        $this->owner = $owner;
        $this->server = $server;
        $this->owner->set_etcjs($this);
        $this->handle = curl_init();
    }
    public function getConfiguration($name)
    {
        $config = new Configuration($name);
        $config->set_etcjs($this);
        return $config;
    }
    public function post($path, $type, $array)
    {
        $array = array("owner" => $this->owner->name,
            "key" => $this->owner->key) + $array;
        $url = "http://".$this->server->host_name.":"
            .$this->server->port."/".$path;
        curl_setopt($this->handle, CURLOPT_URL, $url);
        curl_setopt($this->handle, CURLOPT_POSTFIELDS,
            http_build_query($array));
        curl_setopt($this->handle, CURLOPT_RETURNTRANSFER, true);
        $result = curl_exec($this->handle);
        $error_code = curl_getinfo($this->handle, CURLINFO_HTTP_CODE); 
        return new Result($error_code == 200?$type:Etcjs::ERROR, $result);
    }
    public function cleanup()
    {
        curl_close($this->handle);
    }

}
?>
