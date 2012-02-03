<?
echo "Are you sure you want to $action ?";
echo "<br/>";
echo Form::Open(null, 'GET');
echo Form::hidden('force', 'true');
echo Form::Submit('Yes');
echo Form::Close();
echo Form::Open($back, 'GET');
echo Form::Submit('No');
echo Form::Close();
?>
