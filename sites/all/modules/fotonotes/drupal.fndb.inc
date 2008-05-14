<?php
// $Id: drupal.fndb.inc,v 1.1.4.4 2008/04/07 22:53:04 romka Exp $
function decode_unicode($str){
  $res = '';

  $i = 0;
  $max = strlen($str) - 6;
  while ($i <= $max)
  {
    $character = $str[$i];
    if ($character == '%' && $str[$i + 1] == 'u')
    {
      $value = hexdec(substr($str, $i + 2, 4));
      $i += 6;

      if ($value < 0x0080) // 1 byte: 0xxxxxxx
        $character = chr($value);
      else if ($value < 0x0800) // 2 bytes: 110xxxxx 10xxxxxx
        $character =
            chr((($value & 0x07c0) >> 6) | 0xc0)
          . chr(($value & 0x3f) | 0x80);
      else // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        $character =
            chr((($value & 0xf000) >> 12) | 0xe0)
          . chr((($value & 0x0fc0) >> 6) | 0x80)
          . chr(($value & 0x3f) | 0x80);
    }
    else
      $i++;

    $res .= $character;
  }

  return $res . substr($str, $i);
}

//  FNSave - abstract class to storing annotations
class FNSave {

  function FNSave() { }

  function saveNewAnnotation() { }

  function updateExistingAnnotationByID() { }

  function deleteAnnotationByID() { }
}


//  FNSaveDatabaseRows - concrete class to save in database, annotations stored in separate rows
class FNSaveDatabaseRows extends FNSave {

  function FNSaveDatabaseRows() {
  }

  function saveNewAnnotation($fn_image, $fn_annotation) {
    $content = decode_unicode($fn_annotation->param['content']);
    $xml = decode_unicode($fn_annotation->param['src_xml']);
    global $user;
    $nid = $_SESSION['nid'];
    $result = db_query("INSERT INTO {fotonotes} (nid, uid, annotation_id, title, content, xml) VALUES (%d, %d, '%s', '%s', '%s', '%s')", $nid, $user->uid, $fn_annotation->param['id'], decode_unicode($fn_annotation->param['title']), $content, $xml);
    return true;
  }

  function updateExistingAnnotationByID($fn_image, $fn_annotation) {
    $result = false;
    $content = decode_unicode($fn_annotation->param['content']);
    $xml = decode_unicode($fn_annotation->param['src_xml']);
    $z = substr($fn_annotation->param["id"], strrpos($fn_annotation->param["id"], "/") + 1);
    if (strpos($z, "@")) {
      //$z = substr($z, 0, strpos($z, "@"));
    }
    global $user;
    if (user_access('fotonotes edit all notes')) {
      $result = db_query("UPDATE {fotonotes} SET title='%s', content='%s', xml='%s' WHERE annotation_id='%s'", decode_unicode($fn_annotation->param['title']), $content, $xml, $z);
    }
    else if (user_access('fotonotes edit own notes')) {
      $result = db_query("UPDATE {fotonotes} SET title='%s', content='%s', xml='%s' WHERE annotation_id='%s' AND uid = %d", decode_unicode($fn_annotation->param['title']), $content, $xml, $z, $user->uid);
    }
    return $result;
  }

  function deleteAnnotationByID($fn_image, $fn_annotation) {
    $result = false;
    $z = substr($fn_annotation->param["id"], strrpos($fn_annotation->param["id"], "/") + 1);
    if (strpos($z, "@")) {
      //$z = substr($z, 0, strpos($z, "@"));
    }
    global $user;
    if (user_access('fotonotes edit all notes')) {
      $result = db_query("DELETE FROM {fotonotes} WHERE annotation_id = '%s'", $z);
    }
    else if (user_access('fotonotes edit own notes')) {
      $result = db_query("DELETE FROM {fotonotes} WHERE annotation_id = '%s' AND uid = %d", $z, $user->uid);
    }
    return $result;
  }
}

//  FNRetrieve - abstract class to retrieve entries
class FNRetrieve {

  function FNRetrieve() { }

  function getAnnotations() { }

  function getAnnotationsByID() { }
}

// FNRetrieveDatabaseRows - concrete class to fetch from database, annotations stored in separate rows
class FNRetrieveDatabaseRows extends FNRetrieve {

  function FNRetrieveDatabaseRows() {
  }

  function getAnnotations(&$fn_image) {
      global $DHTML_MAXWIDTH, $DHTML_MAXHEIGHT;

      $nid = str_replace('inote', '', $fn_image->param['nid']);
      if (isset($nid) && $nid != '' && $nid != 'null') {
        $node = node_load($nid);

        $this->image = file_create_path($node->images['preview']);
        $size = getimagesize($this->image);
        displayDebugParam($size, 4);
        $ratioWidth = $DHTML_MAXWIDTH /  $size[0];
        $ratioHeight = $DHTML_MAXHEIGHT / $size[1];
      }

      if ($ratioHeight>$ratioWidth) {
        $ratio=$ratioWidth;
      }
      else {
        $ratio=$ratioHeight;
      }
      if ($ratio>1) {
        $ratio=1;
      }

      $fn_image->setFnImageParam('scalefactor', $ratio);

      $annoatations = array();
      $xml = '';
      $result = db_query("SELECT * FROM {fotonotes} WHERE nid=%d", $nid);
      while ($row = db_fetch_object($result)) {
        $entry = $row->xml;
        preg_match('#<fn:boundingBox>(.*)</fn:boundingBox>#Umsi', $entry, $coordstring);
        $coords = explode(',', $coordstring[1]);
        list ($annotation['upperleftx'],
              $annotation['upperlefty'],
              $annotation['lowerrightx'],
              $annotation['lowerrighty'])  = $coords;
        $annotation['width'] = ($coords[2] - $coords[0]) * $ratio;
        $annotation['height'] = ($coords[3] - $coords[1]) * $ratio;
        $annotation['upperlefty'] *= $ratio;
        $annotation['upperleftx'] *= $ratio;
        $annotation['lowerrightx'] *= $ratio;
        $annotation['lowerrighty'] *= $ratio;

        preg_match('#<title>(.*)</title>#Umsi', $entry, $title);
        $annotation['title'] = $title[1];
        preg_match('#<content.*>(.*)</content>#Umsi', $entry, $content);
        $annotation['content'] = $content[1];
        $bbcode = array(
          '/\[url\]([\w\W]+?)\[\/url\]/i',
          '/\[url=(.+?)\]([\w\W]+?)\[\/url\]/i');
        $replace = array(
          ' [<a href='. base_path() .'node/$1>$1</a>]',
          '<a href='. base_path() .'node/$1 target=_blank>$2</a>');
        $annotation['content'] = preg_replace($bbcode, $replace, $annotation['content']);
        preg_match('#<name>(.*)</name>#Umsi', $entry, $author);
        $annotation['author'] = $author[1];
        preg_match('#<created>(.*)</created>#Umsi', $entry, $created);
        $annotation['created'] = $created[1];
        preg_match('#<issued>(.*)</issued>#Umsi', $entry, $issued);
        $annotation['issued'] = $issued[1];
        preg_match('#<modified>(.*)</modified>#Umsi', $entry, $modified);
        $annotation['modified'] = $modified[1];
        preg_match('#<id>(.*)</id>#Umsi', $entry, $id);
        //$annotation['id'] = basename($id[1]);  Do not get basename, use full url
        $annotation['id'] = $id[1];
        preg_match('#<userid>(.*)</userid>#Umsi', $entry, $userid);
        $annotation['userid'] = $userid[1];
        $annotations[] = $annotation;
      }
      return $annotations;
  }

  function getAnnotationsByID() {
  }
}
