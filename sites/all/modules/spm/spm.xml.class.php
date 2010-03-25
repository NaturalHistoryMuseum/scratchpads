<?php

/**
 * Thanks to the code provided by massimo71 on php.net for this class
 *
 */
class EOLXML extends XMLWriter{

  /**
   * Constructor.
   * @param string $prm_rootElementName A root element's name of a current xml document
   * @param string $prm_xsltFilePath Path of a XSLT file.
   * @access public
   * @param null
   */
  public function __construct(){
    $this->openMemory();
    $this->setIndent(true);
    $this->setIndentString(' ');
    $this->startDocument('1.0', 'UTF-8');
    if($prm_xsltFilePath){
      $this->writePi('xml-stylesheet', 'type="text/xsl" href="' . $prm_xsltFilePath . '"');
    }
    $this->startElement('response');
    $this->writeAttribute('xmlns','http://www.eol.org/transfer/content/0.3');
    $this->writeAttribute('xmlns:xsd','http://www.w3.org/2001/XMLSchema');
    $this->writeAttribute('xmlns:dc','http://purl.org/dc/elements/1.1/');
    $this->writeAttribute('xmlns:dcterms','http://purl.org/dc/terms/');
    $this->writeAttribute('xmlns:geo','http://www.w3.org/2003/01/geo/wgs84_pos#');
    $this->writeAttribute('xmlns:dwc','http://rs.tdwg.org/dwc/dwcore/');
    $this->writeAttribute('xmlns:xsi','http://www.w3.org/2001/XMLSchema-instance');
    $this->writeAttribute('xmlns:lds','http://lifedesk.lifedesks.org/');
    $this->writeAttribute('xsi:schemaLocation', 'http://www.eol.org/transfer/content/0.3 http://services.eol.org/schema/content_0_3.xsd');
  }

  /**
   * Set an element with a text to a current xml document.
   * @access public
   * @param string $prm_elementName An element's name
   * @param string $prm_ElementText An element's text
   * @return null
   */
  public function setElement($tag, $data){
    $this->startElement($tag);
    $this->text($data);
    $this->endElement();
  }

  /**
   * Construct elements and texts from an array.
   * The array should contain an attribute's name in index part
   * and a attribute's text in value part.
   * @access public
   * @param array $prm_array Contains attributes and texts
   * @return null
   */
  public function fromArray($prm_array){
    if(is_array($prm_array)){
      foreach($prm_array as $index => $element){
        if(is_array($element)){
          if(isset($element['element_name'])){
            $index = $element['element_name'];
            unset($element['element_name']);
          }
          $this->startElement($index);
          if(isset($element['attributes'])){
            foreach($element['attributes'] as $key => $value){
              $this->writeAttribute($key, $value);                            
            }
            unset($element['attributes']);
          }
          $this->fromArray($element);
          $this->endElement();
        }else
          $this->setElement($index, $element);
      }
    }
  }

  /**
   * Return the content of a current xml document.
   * @access public
   * @param null
   * @return string Xml document
   */
  public function getDocument(){
    $this->endElement();
    $this->endDocument();
    return $this->outputMemory();
  }

  /**
   * Output the content of a current xml document.
   * @access public
   * @param null
   */
  public function output(){
    header('Content-type: text/xml');
    echo $this->getDocument();
  }
}