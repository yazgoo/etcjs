<?
echo $header;
foreach($configurations as $configuration)
{
    foreach(array(
        'delete' => 'x',
        'edit' => "$configuration",
        'stat' => '(stat)',
        'download' => '(download)') as $action => $title)
        echo HTML::link('config/'.$action.'/'.HTML::entities($configuration),
            "$title")."&nbsp;";
    echo "<br/>";
}
echo $footer;
?>
