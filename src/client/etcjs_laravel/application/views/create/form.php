<?
echo "Please enter the configuration name:";
echo "<br/>";
echo Form::Open(null, 'GET');
echo Form::text('name', '');
echo Form::Submit('Send');
echo Form::Close();
?>
