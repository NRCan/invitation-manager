# Invitation Manager

A Drupal implementation of Principal Publisher's Invitation Manager

This is a Drupal 7.x module that implements Principal Publisher's [Invitation manager](https://github.com/ServiceCanada/invitation-manager)

The module isn't super polished but has a couple features:
 - It can load the library directly from the PP github account
 - It can use its copy of the library (which is the only thing that works at the moment because of some bugs)
 - It presents a UI for saving the .json config file
 - It offers a list of pages and roles who get the library included
 - It saves previous copies of the .json file in a directory for recovery if needed
 - Visit ad/min/structure/invitation_manager for the admin page

This module was written to work with NRCan's Drupal 7 instance which, while using the GCWeb theme is not a WxT site. It uses node translation (seperate nids) and other dated aproaches. This module should work for you but it might not.
