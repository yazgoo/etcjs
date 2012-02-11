<?php
require_once '../application/models/etcjs.php';
class OwnerUser extends Owner
{
    public $id = array();
    public function __construct($id)
    {
        parent::__construct($id["user"], $id["password"]);
        $this->id = $id;
    }
}
return array(
	'username' => 'username',
	'user' => function($id)
	{
        if($id == null) return null;
        return new OwnerUser($id);
	},
	'attempt' => function($username, $password, $config)
	{
        return new OwnerUser(array("user" => $username,
            "password" => $password));
	},
	'logout' => function($user) {}
);
