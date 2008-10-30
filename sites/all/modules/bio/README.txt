-----------------------------------------------------------
bio.module for Drupal
  by Jeff Robbins
    jeff -/at\- lullabot.com
-----------------------------------------------------------

The bio module is a simple custom node-type module with a few "extras".

1) Unless a user has "administer nodes" permissions, he or she can only create *one* bio entry.
2) Any content that a user creates will receive a link to their bio entry.

The module does not use any custom fields. It installs one small database table for associating bio nodes with user ids.

When configuring a site using this module, I recommend either:
a) going to admin/build/themes/settings and disabling the "Display post information on" checkboxes
and/or
b) disabling "access user profiles" for non-admin site visitors on the admin/user/access page so that there is not both a link to the user's account profile page and their bio.

-----------------------------------------------------------
Views Integration
-----------------------------------------------------------

The bio module now boasts integration with the views module. This integration takes two forms:

1. Custom filters: Bio has two useful filters, Node: Type is bio and Bio: Author has bio node
- Node: Type is bio node -- This is useful for shipping premade views that rely on bio, but are not tied to a particular type of node acting as the bio node (e.g. on one site it might be "Biography" on another "profile").
- Bio: Author has bio node -- This is useful for only returning results where the author has created a bio node. That way, if you are exposing bio fields (e.g. the "interests" taxonomy), you can be sure that a bio node exists for every other node returned in the query.

2. Duplicating existing views tables, fields, filters, and sorts: Bio "steals" the views definitions of every other module and makes them its own. For example, the node module exposes the "Node: Title" field and filters. Bio steals these and creates the "Bio: Node: Title". Using these fields you can create a view that returns a list of blog nodes with the author's bio node title. Don't worry, it sounds complicated at first, but you'll get the hang of it in no time.

Bio ships two default views that show the power of these two features:

1. recent_biographies: This view uses the "Node: Type is bio" filter to give you a listing of recent bio nodes. But don't worry, if you have created a custom "Profile" or "My History" content type, it will still work.

2. tracker_bio: Just like the usual tracker view, but with a twist! Instead of listing the node author's user name, this view uses the "Bio: Node: Title" field to list the author's bio node title. This works great for sites where users enter their names in their bio titles.

If you find a problem with a particular view field, filter, or sort please post a bug in the bio module issue and also post a bug linking back to that issue in the original modules queue. Since bio doesn't create the tables, filters, fields, or sorts itself chances are the bug is elsewhere, but we'll work with other module creators to make their modules bio compliant.