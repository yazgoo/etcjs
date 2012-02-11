<?
echo $header;
echo Lang::line('config.please enter name')->get();
echo ":<br/>";
echo Form::Open(null, 'GET');
echo Form::text('name', '');
echo Form::Submit(Lang::line('config.create')->get());
echo Form::Close();
echo $footer;
?>
