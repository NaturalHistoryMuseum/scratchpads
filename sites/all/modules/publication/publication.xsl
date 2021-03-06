<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  xmlns:tp="http://www.plazi.org/taxpub">
  
  <!--  TEMPLATE FOR "P" -->
  <xsl:template match="p">
	  <p>
	    <xsl:for-each select="node()">
		    <xsl:choose>
		    <xsl:when test="self::text()">
		      <xsl:value-of select="."/>
		    </xsl:when>
		    <xsl:when test="self::*">
		      <xsl:apply-templates select="."/>
		    </xsl:when>
	      </xsl:choose>
      </xsl:for-each>
	  </p>
  </xsl:template>
  
  <!-- TEMPLATE TO SHUT UP THINGS -->
  <xsl:template match="tp:taxon-name"/>
  <xsl:template match="tp:taxon-authority"/>
  <xsl:template match="label"/>
  
  <xsl:template match="title">
    <h1><xsl:value-of select="."/></h1>
  </xsl:template>
            
  <!-- TEMPLATE FOR <italic> -->
  <xsl:template match="italic">
    <em><xsl:value-of select="."/></em>
  </xsl:template>
  
    <!--  TEMPLATE FOR <strong> -->
  <xsl:template match="strong">
    <strong>
      <xsl:value-of select="."/>
    </strong>
  </xsl:template>
  
  <!-- TEMPLATE FOR ARTICLE TITLE -->
  <xsl:template match="article-title">
    <xsl:for-each select="node()">
      <xsl:choose>
      <xsl:when test="self::text()">
        <xsl:value-of select="."/>
      </xsl:when>
      <xsl:when test="self::*">
        <xsl:apply-templates select="."/>
      </xsl:when>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>

  <!-- TEMPLATE FOR A SECTION -->
  <xsl:template match="sec">
    <div class="section">
      <xsl:apply-templates/>
      <xsl:apply-templates select="taxon-treatment"/>
    </div>
  </xsl:template>
  
  <!-- TEMPLATE FOR A TREATMENT SECTION -->
  <xsl:template match="tp:treatment-sec">
    <div class="section">
      <xsl:apply-templates/>
      <xsl:apply-templates select="taxon-treatment"/>
      <xsl:apply-templates select="sec"/>
    </div>
  </xsl:template>
  
  <!--  TEMPLATE FOR ACKNOWLEDGEMENTS -->
  <xsl:template match="ack">
   <xsl:if test="p">
    <div class="section">
      <h1>Acknowledgements</h1>
      <xsl:for-each select="p">
            <xsl:choose>
      <xsl:when test="self::text()">
        <p><xsl:value-of select="."/></p>
      </xsl:when>
      <xsl:when test="self::*">
        <xsl:apply-templates select="."/>
      </xsl:when>
      </xsl:choose>

      </xsl:for-each>
    </div>
    </xsl:if>
  </xsl:template>
  
  <!-- TEMPLATE FOR TAXON-TREATMENT -->
  <xsl:template match="tp:taxon-treatment">
    <div class="section taxon-treatment"> 
    <xsl:for-each select="tp:nomenclature">
      <div class="nomenclature">
	      <h1>
	      <xsl:for-each select="tp:taxon-name">
	        <xsl:value-of select="normalize-space()"/>
	        <xsl:if test="position()!=last()"><xsl:text> </xsl:text></xsl:if>
	      </xsl:for-each>
	      </h1>
      </div>
    </xsl:for-each>
    <xsl:apply-templates/>
    </div>
  </xsl:template>
  
  <!--  TEMPLATE FOR  REFERENCES-->
  <xsl:template match="ref-list">
    <xsl:if test="ref">
    <div class="references section">
      <h3>References</h3>
      <ul>
	      <xsl:for-each select="ref">
	       <li>
		        <xsl:for-each select="mixed-citation/person-group/name">
		          <xsl:value-of select="surname"/><xsl:text> </xsl:text><xsl:value-of select="substring(given-names,1,1)"/><xsl:if test="position()!=last()">, </xsl:if>
		        </xsl:for-each>
		        <xsl:text> - </xsl:text>
		        <xsl:value-of select="mixed-citation/year"/>
            <xsl:text> - </xsl:text>
            <b>
              <xsl:value-of select="mixed-citation/article-title"/>
            </b>
	        </li>
	      </xsl:for-each>
      </ul>
    </div>
    </xsl:if>
  </xsl:template>
  

  <!--  TEMPLATE FOR FIG GROUP -->
  <xsl:template match="fig">
    <div class="image">
      <h2><xsl:value-of select="label"/></h2>
      <img> 
        <xsl:attribute name="src"><xsl:value-of select="graphic/@xlink:href"/></xsl:attribute>
        <xsl:attribute name="width">400px</xsl:attribute>
      </img>
      <p><xsl:value-of select="caption"/></p>
    </div>
  </xsl:template>
  
  <!-- TEMPLATE FOR THE WHOLE DOCUMENT -->
  <xsl:template match="/article">
    <html>
      <head><title>Publication</title></head>
    <body>
    <div class="publication">
    <xsl:for-each select="front">
      <div class="publication-front">
        <h1 class="title"><xsl:apply-templates select="article-meta/title-group"/></h1>
        <h2 class="authors">
        <xsl:for-each select="article-meta/contrib-group/contrib">
          <xsl:value-of select="name/given-names"/><xsl:text> </xsl:text><xsl:value-of select="name/surname"/><span class="superscript"><xsl:number/></span><xsl:if test="position()!=last()">, </xsl:if>
        </xsl:for-each>
        </h2>
        <div class="affiliations">
          <ol>
          <xsl:for-each select="article-meta/contrib-group/contrib">
            <li><xsl:value-of select="email"/>, <xsl:value-of select="aff"/></li>
          </xsl:for-each>
          </ol>
        </div>
        <div class="abstract">
	        <h3>Abstract:</h3>
		      <xsl:for-each select="article-meta/abstract">
		        <xsl:apply-templates/>
		      </xsl:for-each>
	        <div class="keywords">
            <h3>Keywords:</h3>
	          <p>
	            <xsl:for-each select="article-meta/kwd-group/kwd">
	              <xsl:value-of select="."/><xsl:if test="position()!=last()">, </xsl:if>
	            </xsl:for-each>
	          </p>
	        </div>
	      </div>
      </div>
    </xsl:for-each>
    <xsl:for-each select="body">
      <div class="publication-body">
        <xsl:apply-templates/>
      </div>
    </xsl:for-each>
    <xsl:for-each select="back">
      <div class="publication-back">
        <xsl:apply-templates/>
      </div>
    </xsl:for-each>
    </div>
    </body>
    </html>
  </xsl:template>
</xsl:stylesheet>