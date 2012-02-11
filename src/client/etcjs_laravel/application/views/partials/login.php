<?
echo $header;
echo Lang::line('client.login')->get();
echo ":";
echo HTML_::br();
echo Form::Open(null, 'POST');
echo Form::text('user', '');
echo HTML_::br();
echo Form::password('password');
echo HTML_::br();
echo Form::Submit(Lang::line('client.login action')->get());
echo Form::Close();
echo $footer;
?>
