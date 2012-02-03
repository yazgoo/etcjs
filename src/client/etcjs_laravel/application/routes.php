<?
function wrap($view)
{
    $view->nest('header', 'partials.header');
    return $view->nest('footer', 'partials.footer');
}
function on_result($result, $callback)
{
    
    if($result->type != Etcjs::ERROR)
    {
        $view = $callback($result->result);
        if($view == null) return null;
        if(get_class($view) == 'Laravel\View') return wrap($view);
        return $view;
    }
    else
        return View::make('partials.error')->with('error', $result->result);
}
function on_get_configuration($name, $callback)
{
    $etcjs = new Etcjs(new Owner("yazgoo", "foo"), new Server("localhost", 1337));
    $configuration = $etcjs->getConfiguration($name);
    $result = $callback($configuration);
    $etcjs->cleanup();
    return $result;
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
return array
(
    'GET /' => function()
    {
        $view = wrap(View::make('home.index'));
        return $view;
    },
    'GET /config/list' => function()
    {
        return on_get_configuration("any", function($configuration) {
            return on_result($configuration->list_(), function($result) {
                return View::make('list.index')
                    ->with('configurations', $result);
            });
        });
    },
    'GET /config/edit/(:any)' => function($name)
    {
        return on_get_configuration($name, function($configuration) use ($name) {
            return on_result($configuration->get(), function($result) use ($name) {
                return  wrap(View::make('edit.index'))
                    ->with('content', $result)->with('name', $name);
            });
        });
    },
    'GET /config/download/(:any)' => function($name)
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
        'GET /config/delete/(:any)'
        => array('before' => 'confirm:delete this file,config/list', function($name)
    {
        return on_get_configuration_redirect_to($name, 'config/list',
            function($configuration) { return $configuration->delete(); });
    }),
    'GET /config/create' => function()
    {
        print_r(Input::get('name'));
        $name = Input::get('name');
        if($name == '') return wrap(View::make('create.form'));
        else return Redirect::to('/config/create/'. HTML::entities($name));
    },
    'GET /config/create/(:any)' => function($name)
    {
        return on_get_configuration_redirect_to($name, 'config/list',
            function($configuration) { return $configuration->touch(); });
    },
    'GET /config/stat/(:any)' => function($name)
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
