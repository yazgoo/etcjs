<html>
<?
echo HTML::link('config/list', Lang::line('config.list')->get());
echo "&nbsp;";
echo HTML::link('config/create', Lang::line('config.new')->get());
echo "&nbsp;";
echo HTML::link('logout', Lang::line('client.logout')->get());
?>
    <h1><? echo Lang::line('client.title')->get() ?></h1>
