<?php

/**
 * Thanks to the code provided by massimo71 on php.net for this class
 *
 */
class XmlConstruct extends XMLWriter{

  /**
   * Constructor.
   * @param string $prm_rootElementName A root element's name of a current xml document
   * @param string $prm_xsltFilePath Path of a XSLT file.
   * @access public
   * @param null
   */
  public function __construct($prm_rootElementName, $prm_xsltFilePath = ''){
    $this->openMemory();
    $this->setIndent(true);
    $this->setIndentString(' ');
    $this->startDocument('1.0', 'UTF-8');
    if($prm_xsltFilePath){
      $this->writePi('xml-stylesheet', 'type="text/xsl" href="' . $prm_xsltFilePath . '"');
    }
    $this->startElement($prm_rootElementName);
  }

  /**
   * Set an element with a text to a current xml document.
   * @access public
   * @param string $prm_elementName An element's name
   * @param string $prm_ElementText An element's text
   * @return null
   */
  public function setElement($tag, $data){
    if(strpos($tag, ":")){ // Accept tags in the form of dc:http://dublincore.org/blah/item
      $parts = explode(":", $tag);
      $prefix = array_shift($parts);
      $rest = implode(":", $parts);
      $parts = preg_split("/[#\/]/", $rest);
      $tag = array_pop($parts);
      $uri = substr($rest, 0, strpos($rest, $tag));
      $this->startElementNS($prefix, $tag, $uri);
    } else {
      $this->startElement($tag);
    }
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
          $this->startElement($index);
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