<?
echo $header;
foreach($configurations as $configuration)
    echo HTML::link('config/edit/'.HTML::entities($configuration),
        HTML::entities($configuration)). "<br/>";
echo $footer;
?>
