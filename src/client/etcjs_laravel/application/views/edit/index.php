<?
echo $header;
echo $name;
echo "<br/>";
echo Form::Open('/config/save/'.HTML::entities($name), 'PUT');
echo Form::textarea('content', HTML::entities($content));
echo "<br/>";
echo Form::Submit(Lang::line('config.save')->get());
echo Form::Close();
echo $footer;
?>
