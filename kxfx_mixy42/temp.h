/* Plugin Implementation */

int iInfoPlugin::save_plugin_settings(kSettings &cfg)
{
 if(pgm_id>0)
 {
  char section[128];
  sprintf(section,"pgm_%d",pgm_id);

  cfg.write(section,"info_text",str_param);
 }
 return 0;
}

int iInfoPlugin::load_plugin_settings(kSettings &cfg)
{
 if(pgm_id>0)
 {
  char section[128];
  sprintf(section,"pgm_%d",pgm_id);

  cfg.read(section,"info_text",str_param,sizeof(str_param));
 }
 else
  str_param[0]=0;

 return 0;
}