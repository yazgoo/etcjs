<?
class HTML_ extends HTML
{
    public static function br() { return "<br/>"; }
}
function wrap($view)
{
    $view->nest('header', 'partials.header');
    return $view->nest('footer', 'partials.footer');
}
function try_to_log_in()
{
    if(Input::get('password') != null && Input::get('user') != null)
        return (Auth::attempt(Input::get('user'), Input::get('password')));
    return false;
}
function on_result($result, $callback)
{
    if($result->type == Etcjs::ERROR)
        return View::make('partials.error')->with('error', $result->result);
    $view = $callback($result->result);
    if($view == null) return null;
    if(get_class($view) == 'Laravel\View') return wrap($view);
    return $view;
}
function on_get_configuration($name, $callback)
{
    if (Auth::check())
    {
        $etcjs = new Etcjs(Auth::user(), new Server("localhost", 1337));
        $configuration = $etcjs->getConfiguration($name);
        $result = $callback($configuration);
        $etcjs->cleanup();
        return $result;
    }
    else 
    {
        if(!try_to_log_in()) return wrap(View::make('partials.login'));
        else return on_get_configuration($name, $callback);
    }
}

function on_get_configuration_redirect_to($name, $redirect, $callback)
{
    return on_get_configuration($name, function($configuration)
        use ($name, $redirect, $callback) {
        return on_result($callback($configuration),
            function($result) use ($redirect) {
                return Redirect::to($redirect);
            });
    });
}
function GET_OR_POST($path)
{
    return "GET $path, POST $path";
}
return array
(
    GET_OR_POST('/') => function()
    {
        $view = wrap(View::make('home.index'));
        return $view;
    },
    GET_OR_POST("/logout") => function()
    {
        Auth::logout();
        return Redirect::to('/');
    },
    GET_OR_POST("/config/list") => function()
    {
        return on_get_configuration("any", function($configuration) {
            return on_result($configuration->list_(), function($result) {
                return View::make('list.index')
                    ->with('configurations', $result);
            });
        });
    },
    GET_OR_POST("/config/edit/(:any)") => function($name)
    {
        return on_get_configuration($name, function($configuration) use ($name) {
            return on_result($configuration->get(), function($result) use ($name) {
                return  wrap(View::make('edit.index'))
                    ->with('content', $result)->with('name', $name);
            });
        });
    },
    GET_OR_POST("/config/download/(:any)") => function($name)
    {
        return on_get_configuration($name, function($configuration) use ($name) {
            return on_result($configuration->get(), function($result) use ($name) {
                return Response::make($result, 200, array(
                    'Content-Type' => 'text/plain',
                    'Content-Description' => 'File Transfer',
                    'Content-Disposition' => 'attachment; filename="'.$name.'"'
                ));
            });
        });
    },
    'PUT /config/save/(:any)' => function($name)
    {
        return on_get_configuration_redirect_to($name, 'config/edit/'.$name,
            function($configuration) { return $configuration->set(Input::get('content')); });
    },
        GET_OR_POST("/config/delete/(:any)")
        => array('before' => 'confirm:delete this file,config/list', function($name)
    {
        return on_get_configuration_redirect_to($name, 'config/list',
            function($configuration) { return $configuration->delete(); });
    }),
    GET_OR_POST("/config/create") => function()
    {
        print_r(Input::get('name'));
        $name = Input::get('name');
        if($name == '') return wrap(View::make('create.form'));
        else return Redirect::to('/config/create/'. HTML::entities($name));
    },
    GET_OR_POST("/config/create/(:any)") => function($name)
    {
        return on_get_configuration_redirect_to($name, 'config/list',
            function($configuration) { return $configuration->touch(); });
    },
    GET_OR_POST("/config/stat/(:any)") => function($name)
    {
        return on_get_configuration($name, function($configuration) use ($name) {
            return on_result($configuration->stat(), function($result) use ($name) {
                $lines = split("\n", $result); 
                foreach($lines as $line)
                    echo $line."<br/>";
                // TODO view
                return null;
            });
        });
    },
);
?>
