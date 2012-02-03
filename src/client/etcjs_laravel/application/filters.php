<?
return array(
    'confirm' => function($action, $back)
    {
        if(Input::get('force') != 'true')
            return View::make('partials.confirm')
            ->with('back', $back)->with('action', $action);
    });
?>
