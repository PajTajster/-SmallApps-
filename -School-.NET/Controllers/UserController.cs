using System;
using System.Collections.Generic;
using System.Linq;
using System.Data;
using System.Net;
using System.Web;
using System.Web.Mvc;
using UsersData.DAL;
using UsersData.Models;
using UsersData.ViewModels.User;


namespace UsersData.Controllers
{
    public class UserController : Controller
    {
        private DB DataBase = new DB();

        [Authorize]
        public ActionResult UserDetails(int? id)
        {
            if (id == null)
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);

            DetailsUserVM DetailUser = new DetailsUserVM(DataBase.UserDataBase.Find(id));

            if (DetailUser == null)
                return HttpNotFound();

            return View(DetailUser);
        }

        [Authorize]
        public ActionResult EditUser(int? id)
        {
            if (id == null)
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);

            EditUserVM EditedUser = new EditUserVM(DataBase.UserDataBase.Find(id));

            if (EditedUser == null)
                return HttpNotFound();

            return View(EditedUser);
        }

        [Authorize]
        [HttpPost]
        [ValidateAntiForgeryToken]
        public ActionResult EditUser([Bind(Include = "ID,FName,LName,Age")] User user)
        {
            if (ModelState.IsValid)
            {
                DataBase.Entry(user).State = System.Data.Entity.EntityState.Modified;
                DataBase.SaveChanges();
                return RedirectToAction("ListUsers");
            }
            return View(user);
        }

        [Authorize]
        public ActionResult CreateNewUser()
        {
            return View();
        }
        [HttpPost]
        [ValidateAntiForgeryToken]
        public ActionResult CreateNewUser([Bind(Include = "ID,FName,LName,Age")] User user)
        {
            if (ModelState.IsValid)
            {
                DataBase.UserDataBase.Add(user);
                DataBase.SaveChanges();
                return RedirectToAction("ListUsers");
            }
            
            return View(user);
        }

        public ActionResult ListUsers()
        {
            List<ListUsersVM> UsersList = new List<ListUsersVM>();

            if (DataBase.UserDataBase.ToList() != null)
            {
                foreach ( User user in DataBase.UserDataBase.ToList())
                {
                    ListUsersVM NewUsr = new ListUsersVM(user);
                    UsersList.Add(NewUsr);
                }
            }

            return View(UsersList);
        }

        [Authorize]
        public ActionResult DeleteUser(int? id)
        {
            if (id == null)
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);

            DeleteUserVM DeletedUser = new DeleteUserVM(DataBase.UserDataBase.Find(id));
            if (DeletedUser == null)
                return HttpNotFound();

            return View(DeletedUser);
        }

        [HttpPost, ActionName("DeleteUser")]
        [ValidateAntiForgeryToken]
        public ActionResult DeleteConfirmation(int id)
        {
            User DeletedUser = DataBase.UserDataBase.Find(id);
            DataBase.UserDataBase.Remove(DeletedUser);
            DataBase.SaveChanges();
            return RedirectToAction("ListUsers");
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
                DataBase.Dispose();
            base.Dispose(disposing);
        }
    }
}