#from subprocess import check_output,STDOUT
from commands import getoutput
from os import path,chdir,listdir,getcwd

#modified for ancients versions of python


debug = False
main_directory="/home/sferes2/doc_sferes"
mv_exec = "/bin/mv"
ln_exec = "/bin/ln"
svn_exec = "/usr/bin/svn"
svn_addr = "https://hotline.isir.upmc.fr/svn"
main_addr = svn_addr + "/sferes2"
nn2_path = "modules/nn2"
evoneuro2_path = "modules/evoneuro2"
archive_path = "binaries"
username = "doc_sferes"
trunk_path = main_directory + "/" + "sferes2/trunk"
download_path = main_directory + "/" + "org/html/bin"



def execute(command):
  if debug:
    pr_str=""
    for element in command:
      pr_str+=element + " " 
    print pr_str
  else:
    pr_str=""
    for element in command:
      pr_str+=element + " " 
    return getoutput(pr_str)
#    return check_output(command)

if __name__ == "__main__":
  #update source code
  if path.isdir("sferes2"):
    execute([svn_exec, "update" , "sferes2", "--username", username, main_directory + "/sferes2"])
  else:
    execute([svn_exec, "checkout" , main_addr, "--username", username, main_directory + "/sferes2"])
  if path.isdir("sferes2/trunk/" + nn2_path):
    execute([svn_exec, "update" ,"sferes2/trunk/" + nn2_path, "--username", username, main_directory+"/sferes2/" + nn2_path])
  else:
    execute([svn_exec, "checkout" , svn_addr + "/er/src/sferes/" + nn2_path, "--username", username, trunk_path + "/" + nn2_path])

  if path.isdir("sferes2/trunk/" + evoneuro2_path):
    execute([svn_exec, "update" ,"sferes2/trunk/" + evoneuro2_path, "--username", username, main_directory+"/sferes2/" + evoneuro2_path])
  else:
    execute([svn_exec, "checkout" , svn_addr + "/er/src/sferes/" + evoneuro2_path, "--username", username, trunk_path + "/" + evoneuro2_path])

  #get svn version
  chdir( trunk_path+ "/"+ nn2_path )
  nn2_ver = execute(["svnversion","-n"]) 
  chdir( trunk_path )
  main_ver = execute(["svnversion","-n"]) 
  #create archive
  execute(["./waf", "dist"])
  #move file and change its name
  filenames = listdir("./")
  for filename in filenames:
    if filename.find("sferes2")!=-1:
      if filename.find("tar.bz2")!=-1:
        mod_filename = filename.replace( main_ver, main_ver + "." + nn2_ver )
        execute([mv_exec,filename, download_path + "/" + mod_filename])
        execute([ln_exec,"-sf","./" +mod_filename, download_path + "/" + "sferes2.latest.tar.bz2"])
        
  

  
