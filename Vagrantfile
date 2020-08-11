# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  # All Vagrant configuration is done here. The most common configuration
  # options are documented and commented below. For a complete reference,
  # please see the online documentation at vagrantup.com.

  config.vm.define "coin-node.multicoin.co", autostart: true, primary: true do |t|
    t.vm.box = "generic/debian9"
    t.vm.synced_folder "./", "/home/node"

    t.vm.provision "ansible" do |ansible|
      ansible.playbook = "./contrib/provisioning/ansible/playbook.yml"
      ansible.extra_vars = { server_ip: "192.168.33.90" }
      ansible.verbose = 'v'
      ansible.groups = {
        "vagrant" => ["coin-node.multicoin.co"]
      }
    end

    t.vm.provider "virtualbox" do |v|
      v.memory = 1024
      v.cpus = 2
    end

    t.vm.network :private_network, ip: "192.168.33.90"

    t.vm.hostname = "coin-node.multicoin.co"
  end

end
