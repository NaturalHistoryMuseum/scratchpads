Drupal node_import/supported/ecommerce README.txt
==============================================================================

This directory contains support files for the ecommerce module bundle.

Thanks to these support files, node_import is able to import following
ecommerce products:
 - tangible (shippable product).

Code explanation
------------------------------------------------------------------------------

The products family of node content types have one big problem: the
$node->type is always set to 'product' for each ecommerce product. The
product.module itself does not support any node content types by itself,
rather other modules extend the basic functionality of product.module.

These extented products get their product-name in $node->ptype rather
then $node->type.

In order for node_import to work, ecommerce product support files need to
do the following:

1. In hook_node_import_types() return a list of product types yourproduct
   module provides where you set $type equals to $node->ptype. So instead
   of returning:

   array('product' => t('Your product');

   you need to return

   array('yourproduct' => t('Your product');

   even though $node->type == 'product' and $node->ptype == 'yourproduct'.

2. In hook_node_import_fields() only return a list of fields that the
   yourproduct module provides; do not include the fields product.module
   supports. The $type will be set to 'yourproduct' rather then
   'product' here.

3. In hook_node_import_global() $type will be set to 'yourproduct'. Again
   only provide the options you need for yourproduct without including
   the ones provided by product.module.

4. In hook_node_import_static() $type will be set to 'yourproduct'.

5. In hook_node_import_prepare() you can assume that $node->type will be
   set to 'product' and $node->ptype to 'yourproduct'.

The reason this will work is because $type is only used internally inside
node_import for most hooks. In addition, product.inc will do some magic
behind the scene:

1. In product_node_import_static() we will copy $node->type into
   $node->ptype and set $node->type to 'product'. This makes it so that
   all things are "normal" in hook_node_import_prepare().

2. In product_node_import_fields() we will invoke for $type = 'product'
   all hook_node_import_fields() of other modules. Eg, taxonomy.module
   assumes that vocabularies are attached to the 'product' node type
   rather then the 'yourproduct' node type (which does not exist).

3. In product_node_import_global() we do the same.

4. In product_node_import_static() we do the same.

$Id: README.txt,v 1.2 2006/09/22 07:21:05 robrechtj Exp $
