Login Security
==============

This module was developed to improve the security options in the login operation
of a Drupal site. By default, Drupal used only basic access control - denying
IP access to the full content of the site after manual intervention.

As Drupal grew older, flood control was introduced as a core feature. While I
was migrating this module from Drupal to Backdrop, I was repeatedly blocked in
my testing by features of the built-in flood control.

In real-life use of this module with Drupal, I also found that frequently the
core flood control features would engage before Login Security. Consider
relying on flood control for your login security - I believe it obsoletes this
module.

Flood Control Features
----------------------

- Users are automatically blocked (temporarily) for 5 incorrect login attempts
within a 1 hour timeframe.
- IP addresses are blocked (temporarily) for 50 incorrect login attempts within
an hour, regardless of the username submitted.

Current "Maintainer"
--------------------

- David Norman (https://github.com/deekayen)


Credits
-------

- Originally written for Drupal by ilo
  (https://www.drupal.org/project/login_security)
- Placeholder for Backdrop by David Norman (https://github.com/deekayen)


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
