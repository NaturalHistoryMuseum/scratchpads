Fotonotes  – collection of scripts for annotating images. Fotonotes is very simple. Only 2 steps and all done. First, you have to select a part of image and write comment linked to selected part. Next, when user place mouse cursor over noted part of image, desire note will be showed. In such a way very convenient to select friends on collective photos, or components of complex schemes. This feature is common used, for example, on Flickr.com website.

Now Fotonotes available in the form of module for Drupal 5 and 6.

Features:

    * Fotonotes module integrates with Image module. It adds tab "Edit fotonotes" for all images. You may add notes for choosed image using that tab.
    * Access control system allows to customize note permissions for any image. For example, moderators will may add-delete-edit notes for all images, advanced users will may add-delete their own notes for all images, but simple users will may add notes only for their images.
    * There was error correction for Cyrillic fonts implemented.
    * Also, any amount of images with notes may be added into any content using bb-code [inotå=%] (sign “%” is replacing with nid-picture includes notes).

Install:

    * Download this archive for Drupal 6, or this for Drupal 5
    * unpack it to your "modules" folder
    * Enable module in "/admin/build/modules" (you must install and enable Image module first)
    * Check "Fotonotes filter, noted image inclusion to node" for necessary image formats ("/admin/settings/filters"). After this action activate bb-code [inotå=%] will function properlyfor selected input format.
    * Set necessary access permissions in "/admin/user/permissions".

That’s all, now you and your visitors may use notes for all images.

Demo: http://fotonotes-en.romka.eu
			http://fotonotes.romka.eu