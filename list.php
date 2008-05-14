<?php
header("Content-Type: text/html;charset=ISO-8859-1");

if (isset($_GET['modules'])){
  $files = scandir("/var/www/html/sites/all/modules");
  $modules = array();
  foreach($files as $file){
  	if (substr($file,0,1)!="."){
  		$modules[] = $file;
  	}
  }
  ?>
  document.write("<ul>
  <?php
  foreach ($modules as $module){
  	echo '<li><a href="http://drupal.org/project/'.$module.'">'.$module.'</a></li>'; 
  }
  ?></ul>");<?php
}
else{
  // Firstly scan the directory for folders, these will form the basis of the 
  // list
	if (isset($_GET['missions'])){
		$_GET['mission']="";
	}	
	$files = scandir("/var/www/html/sites");
	$domains = array();
	for ($i=2;$i<count($files);$i++){
	  if (substr($files[$i],0,3)!="www"
	    && $files[$i]!="all"
            && $files[$i]!=".svn"
	    && $files[$i]!="default"
	    && $files[$i]!="default.myspecies.info"
	    && $files[$i]!="edit.nhm.ac.uk"
	    && $files[$i]!="scratchpads.eu"
	    && $files[$i]!="bibref.editwebrevisions.info"
            && substr($files[$i],0,8) != 'training'
	    && $files[$i]!="sandbox.scratchpads.eu"
	    && is_dir("/var/www/html/sites/".$files[$i])
	  )
	    $domains[]=$files[$i];
	}
	// Connect to the database as ROOT, so that we can get all the details from all the sites - Man, I feel dirty
	$database_details = parse_ini_file('/var/www/drupal_db_passwords',true);
	mysql_connect("localhost", "root", $database_details['root']['password'], TRUE, 2);
	if(isset($_GET['thumbnails'])){
	  ?>document.write('<style type="text/css">.listhidden{display:none}div.listnothidden{display:block}</style><h3><a onclick="prevBlock(\'allsites\');" id="prevscratchpads" style="listhidden">&lt;&lt;&lt;</a> [<a onclick="sortDivs(\'allsites\',\'nodes\');">Nodes</a> | <a onclick="sortDivs(\'allsites\',\'domain\');">Domain</a>] <a id="nextscratchpads" style="listnothidden" onclick="nextBlock(\'allsites\');">&gt;&gt;&gt;</a></h3><div id="allsites"><?php
	  $number_visible = 15;
	  $visible_count = 0;
	  foreach ($domains as $domain){
	    $short_domain = str_replace('-','',array_shift(explode('.',$domain)));
	    mysql_select_db($short_domain); // Do I need to do this if I specify a database in the select statement. DUMB!
	    $nodes = array_pop(mysql_fetch_array(mysql_query("SELECT COUNT(nid) AS nodes FROM node;")));
      $users = array_pop(mysql_fetch_array(mysql_query("SELECT COUNT(uid) AS users FROM users")));
      $views = array_pop(mysql_fetch_array(mysql_query("SELECT SUM(totalcount) AS totalcount FROM node_counter;")));
	    $site_title = htmlspecialchars(unserialize(array_pop(mysql_fetch_array(mysql_query("SELECT value FROM variable WHERE name='site_name';")))),ENT_QUOTES);
      echo '<div style="float:left;height:270px;width:300px;" ';
      if($visible_count>=$number_visible){
        echo 'class="listhidden" ';
      }else{
        echo 'class="listnothidden" ';
      }
      echo 'nodes="'.$nodes.'" domain="'.$domain.'"><a href="http://'.$domain.'"><img id="img'.$short_domain.'" src="http://quartz.nhm.ac.uk/screenshots/'.$domain.'.280x210-drop.png" style="border:0;padding:0;margin:0;" onMouseOver="mouseOverScreenshots(\\\'img'.$short_domain.'\\\',image'.$short_domain.');" onMouseOut="mouseOverScreenshots(\\\'img'.$short_domain.'\\\',originalimage'.$short_domain.');"/></a><br/>'.$site_title.'</div>';
      $visible_count ++;
    }
          ?></div><div class="mainfull"></div>');
<?php
  foreach ($domains as $domain){
	  $short_domain = str_replace('-','',array_shift(explode('.',$domain)));
    echo 'var image'.$short_domain.' = new Image;
var originalimage'.$short_domain.' = new Image;
image'.$short_domain.'.src = "http://quartz.nhm.ac.uk/screenshots/'.$domain.'.280x210-drop.annotated.png";
originalimage'.$short_domain.'.src = "http://quartz.nhm.ac.uk/screenshots/'.$domain.'.280x210-drop.png";
';
  }
?>
var lastSort = 'domain';
function mouseOverScreenshots(from,to){
  document.getElementById(from).src = to.src;}

function nextBlock(parentId){
  divs = document.getElementById(parentId).childNodes;
  var prevFirstShown=1000000
  var prevLastShown=-1
  for(i=0;i<divs.length;i++){
    if(divs[i].className == 'listnothidden'){
      divs[i].className = 'listhidden';
      if(i>prevLastShown){
        prevLastShown = i;}
      if(i<prevFirstShown){
        prevFirstShown = i;}}}
  if(prevLastShown+2+(prevLastShown-prevFirstShown)>divs.length){
    limit = divs.length;
    document.getElementById('nextscratchpads').className = 'listhidden';
  } else {
    limit = prevLastShown+2+(prevLastShown-prevFirstShown);  }
  for(i=prevLastShown+1;i<limit;i++){
    divs[i].className = 'listnothidden';}}

function prevBlock(parentId){
  divs = document.getElementById(parentId).childNodes;
  var prevFirstShown=1000000
  var prevLastShown=-1
  for(i=0;i<divs.length;i++){
    if(divs[i].className == 'listnothidden'){
      divs[i].className = 'listhidden';
      if(i>prevLastShown){
        prevLastShown = i;}
      if(i<prevFirstShown){
        prevFirstShown = i;}}}
  var alertString = '';
  for(i=prevFirstShown-1;i>(prevFirstShown-(prevLastShown-prevFirstShown+2));i--){
    alertString +=' '+i;
    divs[i].className = 'listnothidden';}}

function sortDivs(parentId,sortField){
  divs = document.getElementById(parentId).childNodes;
  if(sortField =='reverse'){
    var oldDivs = new Array();
    for(i=0;i<divs.length;i++){
      var newarray = Array();
      newarray[0] = divs[i].innerHTML
      newarray[1] = divs[i].getAttribute("nodes");
      newarray[2] = divs[i].getAttribute("domain");
      oldDivs[i] = newarray;}
    for(i=0;i<divs.length;i++){
      var newId = divs.length - (i + 1);
      divs[newId].innerHTML=oldDivs[i][0];
      divs[newId].setAttribute("nodes",oldDivs[i][1]);
      divs[newId].setAttribute("domain",oldDivs[i][2]);}}
  if(sortField =='nodes'){
    if(lastSort =='nodes'){
      sortDivs(parentId,'reverse');
      lastSort = '';
      return;
    }
    lastSort = 'nodes';
    var oldDivs = new Array();
    var divsNodesNumbers = new Array();
    for(i=0;i<divs.length;i++){
      divsNodesNumbers[i] = divs[i].getAttribute('nodes');}
    divsNodesNumbers.sort(function(a,b){return a - b});
    var j=0;
    var lastvalue = -1;
    while(value = divsNodesNumbers.pop()){
      if(value!=lastvalue){
        // Get all the sites with this value of nodes
        for(i=0;i<divs.length;i++){
          if(divs[i].getAttribute('nodes') == value){
            var newarray = Array();
            newarray[0] = divs[i].innerHTML
            newarray[1] = divs[i].getAttribute('nodes');
            newarray[2] = divs[i].getAttribute('domain');
            oldDivs[j] = newarray;
            j++;}}}
      lastvalue = value;}
    for(i=0;i<divs.length;i++){
      divs[i].innerHTML = oldDivs[i][0];
      divs[i].setAttribute('nodes',oldDivs[i][1]);
      divs[i].setAttribute('domain',oldDivs[i][2]);}}
  if(sortField =='domain'){
    if(lastSort =='domain'){
      sortDivs(parentId,'reverse');
      lastSort = '';
      return;
    }
    lastSort = 'domain';
    var oldDivs = new Array();
    var divsDomains = new Array();
    for(i=0;i<divs.length;i++){
      divsDomains[i] = divs[i].getAttribute('domain');
    }
    divsDomains.sort();
    var j=0;
    while(value = divsDomains.shift()){
      // Get all the sites with this value of domain
      for(i=0;i<divs.length;i++){
        if(divs[i].getAttribute('domain') == value){
          var newarray = Array();
          newarray[0] = divs[i].innerHTML
          newarray[1] = divs[i].getAttribute('nodes');
          newarray[2] = divs[i].getAttribute('domain');
          oldDivs[j] = newarray;
          j++;
        }
      }
    }
    for(i=0;i<divs.length;i++){
      divs[i].innerHTML = oldDivs[i][0];
      divs[i].setAttribute('nodes',oldDivs[i][1]);
      divs[i].setAttribute('domain',oldDivs[i][2]);
    }
  }
}
<?php
	  // Woot!
	} else {
	
	// We now have a list of domain names, we should use these domain names to 
	// connect to the relevant databases and get the information for each.
	$domain_details = array();
	$array = parse_ini_file('/var/www/drupal_db_passwords',true);
	foreach ($domains as $domain){
	  // get the info from the database, and shove in an array
	  $db_url_part ="";
	  $host_parts = explode(".", $domain);
	  $db_url_part = $host_parts[0];
	  if (strlen($db_url_part)<4)
	    $db_url_part = $host_parts[1];
	  $db_url_part = str_replace("-","",$db_url_part);
	  mysql_connect("localhost", $array[$db_url_part]['user'], $array[$db_url_part]['password'], TRUE, 2);
	  mysql_select_db($db_url_part);
	  $query = "SELECT name,value FROM variable WHERE name='site_name' OR name='site_mission';";
	  $results = mysql_query($query);
	  $site_mission = "";
	  $site_name = "";
	  for ($i=0; $i<2; $i++){
	    $line = mysql_fetch_array($results);
	    if ($line[0]=="site_mission")
	      $site_mission = $line[1];
	    else
	    $site_name = substr(array_pop(split(":",$line[1])),1);
	    $site_name = substr($site_name,0,strlen($site_name)-2);
	  }
	  $domain_details [] = array("domain"=>$domain, "mission"=>$site_mission, "name"=>$site_name);
	  mysql_close();
	}
	// Now we output the javascript which will magically write this to HTML!
	?>
	document.write("<ul><?php
	
	foreach ($domain_details as $domain_detail){
	  $first_invcoms = strpos($domain_detail['mission'],'"')+1;
	  $domain_detail['mission'] = substr($domain_detail['mission'],$first_invcoms);
	  $domain_detail['mission'] = str_replace("\n", "",str_replace("\r", "",str_replace('"','\"',strip_tags(substr($domain_detail['mission'],0,strlen($domain_detail['mission'])-2)))));
	  echo '<li><a href=\'http://'.$domain_detail['domain'].'\'>'.$domain_detail['name'].'</a>';
	  if (isset($_GET['mission']))
	    echo '<ul><li>'.$domain_detail['mission'].'</li></ul>';
	  echo '</li>';
	}
	?></ul>");<?php
	}
}
