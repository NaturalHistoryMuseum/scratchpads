$Id$

Tasks.module by Jake Gordon (jakeg)
A tasklist module

FEATURES

- A collaborative tasklist
- Assigning tasks to users
- Colour coding of tasks by user
- Ordering of tasks
- Tasks completion dates
- Unlimited sub-task(list)s


REQUIREMENTS

- Drupal 4.7
- MySQL as your database engine (for .install file to create required database table)

INSTALLATION

- Copy files to your module directory
- Turn the module on at admin/modules
  - should automatically create teh necessary database table if using MySQL
- Give appropriate permissions at admin/access
- Create your master tasklist at node/add/tasks
- The tasklist is viewable at /tasks, though this just redirects to the URL of your master tasklist node

(TIP: colour code tasks by user by going to user/[userid]/edit and changing your tasklist colour)