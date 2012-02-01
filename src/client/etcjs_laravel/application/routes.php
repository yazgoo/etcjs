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
        if(get_class($view) == 'Laravel\View') return wrap($view);
        return $view;
    }
    else
        View::make('partials.error')->with('error', $result->result);
}
function on_get_configuration($name, $callback)
{
    $etcjs = new Etcjs(new Owner("yazgoo", "foo"), new Server("localhost", 1337));
    $configuration = $etcjs->getConfiguration($name);
    $result = $callback($configuration);
    $etcjs->cleanup();
    return $result;
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
    'PUT /config/save/(:any)' => function($name)
    {
        return on_get_configuration($name, function($configuration) use ($name) {
            return on_result($configuration->set(Input::get('content')),
                function($result) use ($name) {
                    return Redirect::to('config/edit/'.$name);
                });
        });
    },
);
?>
